// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "CPP_WeaponAnimInstance.h"
#include "..\Character\CPP_PlayableCharacter.h"
#include "..\Character\CPP_PGCharacter.h"
#include "..\Character\CPP_A_Hands.h"
#include "Net\UnrealNetwork.h"
#include "..\Core/GrizzlyGameInstance.h"
#include "ProjectGrizzly/Character/CPP_A_PGCharacter.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	bWantsInitializeComponent = true;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponDataTable.WeaponDataTable"));
		check(IsValid(DataTable.Object));
		WeaponDataDT = DataTable.Object;
	}
	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponAnimTable.WeaponAnimTable"));
		check(IsValid(DataTable.Object));
		WeaponAnimDT = DataTable.Object;
	}

	InitTPAnim();
	
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	

	// ...
	// WeaponData = GetWeaponDataTable()->FindRow<FWeaponData>(WeaponName, FString(""));
	// ensure(WeaponData != NULL); // WeaponName �� �ش��ϴ� �����Ͱ� �������� ����
	//
	// WeaponAnim = GetWeaponAnimTable()->FindRow<FWeaponAnim>(WeaponName, FString(""));
	// ensure(WeaponData != NULL); // WeaponName �� �ش��ϴ� �����Ͱ� �������� ����

	//SetWeapon(TEXT("AKM"));

}


void UWeaponComponent::SetWeapon_Implementation(ACPP_WeaponInstance* _WeaponInstance)
{
	CurrentWeaponInstance = _WeaponInstance;
	FWeaponData* _WeaponData = _WeaponInstance->GetWeaponData();
	checkf(_WeaponData != nullptr,TEXT("Cant Find %s in WeaponDT"),*_WeaponInstance->GetWeaponData()->WeaponName);
	// ���� �ݵ� ������
	SetWeapon_Inner(_WeaponInstance);
}

void UWeaponComponent::SetWeaponWithAmmo_Implementation(ACPP_WeaponInstance* _WeaponInstance, ACPP_Ammo* _Ammo)
{
	AmmoInstance = _Ammo;
	SetWeapon(_WeaponInstance);
}

void UWeaponComponent::SetUnarmed_Implementation()
{
	SetWeaponWithAmmo(UnarmedWeaponInstance,nullptr);
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::SetWeapon_Inner(ACPP_WeaponInstance* _WeaponInstance)
{
	CurrentWeaponInstance = _WeaponInstance;
	CurrentWeaponName = FName(*_WeaponInstance->GetWeaponData()->WeaponName);
	// FWeaponData* _WeaponData = Cast<UGrizzlyGameInstance>(GetWorld()->GetGameInstance())->GetWeaponDT()->FindRow<FWeaponData>(_WeaponName, FString(""));
	FWeaponData* _WeaponData = _WeaponInstance->GetWeaponData();
	checkf(_WeaponData != nullptr,TEXT("Cant Find %s in WeaponDT"),*_WeaponInstance->GetWeaponData()->WeaponName);
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	ensure(Character != NULL);
	USkeletalMesh* WeaponMesh = LoadObject<USkeletalMesh>(NULL, *_WeaponData->WeaponSkeletalMesh.ToSoftObjectPath().ToString());
	Character->GetTPWeaponComponent()->SetSkeletalMesh(WeaponMesh);
	//�� �׸���
	Character->GetShadowWeaponComponent()->SetSkeletalMesh(WeaponMesh);
	//1��Ī �Ž�
	Character->GetFPWeaponMeshComponent()->SetSkeletalMesh(WeaponMesh);
	Character->GetFPWeaponMeshComponent()->SetAnimInstanceClass(_WeaponData->WeaponAnimClass);
	//���� ���� ����ó��
	if(_WeaponInstance->GetWeaponData()->WeaponName.Compare(TEXT("Unarmed"))==0)
	{
		Character->GetTPWeaponComponent()->SetVisibility(false);
		Character->GetShadowWeaponComponent()->SetVisibility(false);
		Character->GetFPWeaponMeshComponent()->SetVisibility(false);
		Character->GetHandsMeshComponent()->SetVisibility(false);
	}
	else
	{
		Character->GetTPWeaponComponent()->SetVisibility(true);
		Character->GetShadowWeaponComponent()->SetVisibility(true);
		Character->GetFPWeaponMeshComponent()->SetVisibility(true);
		Character->GetHandsMeshComponent()->SetVisibility(true);
	}
	//�ݵ�
	WeaponRecoilCurve = LoadObject<UCurveVector>(NULL, *_WeaponData->WeaponRecoilCurve.ToSoftObjectPath().ToString());

	//���� �߻���� ���� �ν��Ͻ��� �ű�
	

	//--------------------------------------------------------------------------------------------------------------
	//											Animation
	//--------------------------------------------------------------------------------------------------------------
	FWeaponAnim* WeaponAnim = GetWeaponAnimDT()->FindRow<FWeaponAnim>(CurrentWeaponName,FString(""));
	check(WeaponAnim != nullptr);
	
	//1��Ī ������ ��Ĺ ���
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative,EDetachmentRule::KeepRelative,EDetachmentRule::KeepWorld,true);
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative,EAttachmentRule::KeepRelative,EAttachmentRule::KeepWorld,true);
	if(_WeaponData->SocketMode)
	{
		Character->GetFPWeaponMeshComponent()->DetachFromComponent(DetachmentTransformRules);
		Character->GetFPWeaponMeshComponent()->AttachToComponent(Character->GetHandsMeshComponent(),AttachmentTransformRules,TEXT("RightHand"));
	}
	else
	{
		
		Character->GetFPWeaponMeshComponent()->DetachFromComponent(DetachmentTransformRules);
		Character->GetFPWeaponMeshComponent()->AttachToComponent(Character->GetHandsMeshComponent(),AttachmentTransformRules);
	}
	
	//3��Ī �޼� IK
	UCPP_A_PGCharacter* AnimInstance = Cast<UCPP_A_PGCharacter>(Character->GetMesh()->GetAnimInstance());
	if(_WeaponData->WeaponType == EWeaponType::AssaultRilfe)
		AnimInstance->LeftHandIKAlpha = 1.f;
	else
		AnimInstance->LeftHandIKAlpha = 0.f;

	
	UAnimMontage* WeaponShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_Shoot.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(TEXT("Shoot"), WeaponShoot);
	UAnimMontage* HandsShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Hands_Shoot.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Shoot"), HandsShoot);

	//���� Idle�� Empty�� ���ϴ� �ִϸ��̼��� �����ϸ� �ε�
	if(HasEmptyIdleAnim())
	{
		LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Weapon_EmptyIdle.ToSoftObjectPath().ToString());
		if(IsMagazineEmpty())
		{
			SetEmptyIdleAnim();
		}
	}
	
	//������ �ѹ� �ִϸ��̼��� ������ �߰�
	if(!WeaponAnim->Weapon_LastShoot.IsNull())
	{
		UAnimMontage* WeaponLastShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_LastShoot.ToSoftObjectPath().ToString());
		FP_Weapon_MontageMap.Add(TEXT("Shoot_Last"), WeaponLastShoot);
	}
	UAnimMontage* WeaponLastShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_Shoot.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(TEXT("Shoot"), WeaponShoot);



	LoadFPAnim(WeaponAnim->Weapon_Reload, WeaponAnim->Hands_Reload, TEXT("Reload"));
	LoadFPAnim(WeaponAnim->Weapon_TacReload, WeaponAnim->Hands_TacReload, TEXT("TacReload"));
	//��ο� & ���ο� �ִϸ��̼�
	UAnimMontage* Draw = LoadObject<UAnimMontage>(NULL, *WeaponAnim->AM_Hands_Draw.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Draw"), Draw);
	UAnimMontage* Undraw = LoadObject<UAnimMontage>(NULL, *WeaponAnim->AM_Hands_Undraw.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Undraw"), Undraw);

	//�� ��ġ & ȸ��
	Character->SetIdleHandsLocation(_WeaponData->IdleHandsLocation);
	Character->SetADSHandsLocation(_WeaponData->ADSHandsLocation);
	Character->SetHandsRotation(_WeaponData->HandsRotation);

	//�� �ִϸ��̼� �ʱ�ȭ
	UCPP_A_Hands* HandsAnimInstance = Cast<UCPP_A_Hands>(Character->GetHandsMeshComponent()->GetAnimInstance());
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_Idle.ToSoftObjectPath().ToString());
		HandsAnimInstance->IdleSequence = Anim;
	}
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_WalkForward.ToSoftObjectPath().ToString());
		HandsAnimInstance->MoveForwardSequence = Anim;
	}
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_WalkBackward.ToSoftObjectPath().ToString());
		HandsAnimInstance->MoveBackSequence = Anim;
	}
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_Sprint.ToSoftObjectPath().ToString());
		HandsAnimInstance->SprintSequence = Anim;
	}
}

void UWeaponComponent::OnRep_CurrentWeaponInstance()
{
	SetWeapon_Inner(CurrentWeaponInstance);
}

FWeaponData* UWeaponComponent::GetCurrentWeaponData() const
{
	if(GetWeaponDataDT() == nullptr)
		return nullptr;
	FWeaponData* Data = GetWeaponDataDT()->FindRow<FWeaponData>(CurrentWeaponName,FString(""));
	check(Data != nullptr);
	return Data;
}

FWeaponAnim* UWeaponComponent::GetCurrentAnimData() const
{
	if(GetWeaponAnimDT() == nullptr)
		return nullptr;
	FWeaponAnim* Data = GetWeaponAnimDT()->FindRow<FWeaponAnim>(CurrentWeaponName,FString(""));
	checkf(Data != nullptr,TEXT("%s is illegal"),CurrentWeaponName);
	return Data;
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, UnarmedWeaponInstance);
	DOREPLIFETIME(UWeaponComponent, CurrentWeaponInstance);
	DOREPLIFETIME(UWeaponComponent, AmmoInstance);
	DOREPLIFETIME(UWeaponComponent, bIsInfinityMode);
}


void UWeaponComponent::LoadFPAnim(TSoftObjectPtr<UAnimMontage> _WeaponAnim, TSoftObjectPtr<UAnimMontage> _HandAnim, FName _Name)
{
	UAnimMontage* Weapon = LoadObject<UAnimMontage>(NULL, *_WeaponAnim.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(_Name, Weapon);
	UAnimMontage* Hands = LoadObject<UAnimMontage>(NULL, *_HandAnim.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(_Name, Hands);
}

UAnimMontage* UWeaponComponent::Get_FP_Weapon_Montage(FName _Name) const
{
	UAnimMontage* ReturnValue = nullptr;
	
	//�ѱ��� ��� �ִϸ��̼��� Shoot �ϰ�� ���� LastShoot ����� �����ϰ� ������ �ѹ��� �߻��ϴ� ��Ȳ�̸�
	if(IsMagazineEmpty())
	{
		if(_Name.Compare(TEXT("Shoot")) == 0)
		{
			if(FP_Weapon_MontageMap.Contains(TEXT("Shoot_Last")))
			{
				ReturnValue = *FP_Weapon_MontageMap.Find(TEXT("Shoot_Last"));
			}
			else
			{
				ReturnValue = *FP_Weapon_MontageMap.Find(TEXT("Shoot"));
			}
			return ReturnValue;
		}
	}
	ReturnValue = *FP_Weapon_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name�� �ش��ϴ� �ִϸ��̼��� Map�� �������� ����
	return ReturnValue;
}

UAnimMontage* UWeaponComponent::Get_FP_Hands_Montage(FName _Name) const
{
	UAnimMontage* ReturnValue = *FP_Hands_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name�� �ش��ϴ� �ִϸ��̼��� Map�� �������� ����
	return ReturnValue;
}


UAnimMontage* UWeaponComponent::Get_TP_Montage(FName _Name) const
{
	FString Str = _Name.ToString();
	FString WeaponTypeStr = StaticEnum<EWeaponType>()->GetNameStringByValue((int64)GetWeaponType());
	Str.Append(TEXT("_"));
	Str.Append(WeaponTypeStr);
	_Name = FName(Str);
	UAnimMontage* ReturnValue = *TP_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name�� �ش��ϴ� �ִϸ��̼��� Map�� �������� ����
	return ReturnValue;
}

bool UWeaponComponent::HasEmptyIdleAnim() const
{
	if(!GetCurrentAnimData()->Weapon_EmptyIdle.IsNull())
	{
		return true;
	}
	return  false;
}


void UWeaponComponent::SetAnimIdle_Implementation()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	UCPP_WeaponAnimInstance* AnimInstance = Cast<UCPP_WeaponAnimInstance>(Character->GetFPWeaponMeshComponent()->GetAnimInstance());
	UAnimSequence* IdleAnimation = Cast<UAnimSequence>(GetCurrentAnimData()->Weapon_Idle.Get());
	AnimInstance->SetIdle(IdleAnimation);
}

void UWeaponComponent::SetEmptyIdleAnim_Implementation()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	UCPP_WeaponAnimInstance* AnimInstance = Cast<UCPP_WeaponAnimInstance>(Character->GetFPWeaponMeshComponent()->GetAnimInstance());
	UAnimSequence* EmptyAnimation = Cast<UAnimSequence>(GetCurrentAnimData()->Weapon_EmptyIdle.Get());
	AnimInstance->SetIdle(EmptyAnimation);
}

class UDataTable* UWeaponComponent::GetWeaponDataDT() const
{
	return WeaponDataDT;
}

class UDataTable* UWeaponComponent::GetWeaponAnimDT() const
{
	return WeaponAnimDT;
}

void UWeaponComponent::SetAmmoInstance_Implementation(ACPP_Ammo* _AmmoInstance)
{
	AmmoInstance = _AmmoInstance;
}

void UWeaponComponent::SetCurrentMagazineRounds(int _NewValue)
{
	int CurrentMagazineRounds = FMath::Clamp(_NewValue, 0, GetPerMagazineRounds() + 1);
	CurrentWeaponInstance->SetRounds(CurrentMagazineRounds);
	//�Ѿ��� ������ Empty �ִϸ��̼��� �ִٸ� ����
	if(IsMagazineEmpty() && HasEmptyIdleAnim() && GetOwner()->HasAuthority())
	{
		SetEmptyIdleAnim();
	}
}

void UWeaponComponent::Reload_Inner()
{
	//�Ѿ��� ������ ������ No
	if(AmmoInstance == nullptr)
	{
		return;
	}
	int RemainRounds = GetRemainRounds();
	int CurrentMagazineRounds = GetCurrentMagazineRounds();

	//��Ƽ�� ���ε�
	if (RemainRounds + CurrentMagazineRounds >= GetPerMagazineRounds())
	{
		if (!IsMagazineEmpty())
		{
			RemainRounds += CurrentMagazineRounds;
			if (RemainRounds == GetPerMagazineRounds())
			{
				CurrentMagazineRounds = GetPerMagazineRounds();
			}
			else
			{
				CurrentMagazineRounds = GetPerMagazineRounds() + 1;
			}
			RemainRounds -= CurrentMagazineRounds;
		}
		else
		{
			RemainRounds += CurrentMagazineRounds;
			CurrentMagazineRounds = GetPerMagazineRounds();
			RemainRounds -= CurrentMagazineRounds;
		}
	}
	else
	{
		RemainRounds += CurrentMagazineRounds;
		CurrentMagazineRounds = RemainRounds;
		RemainRounds = 0;
	}

	//���� �ν��Ͻ��� �Ѿ� ����
	CurrentWeaponInstance->SetRounds(CurrentMagazineRounds);
	
	if(!bIsInfinityMode)
	{
		AmmoInstance->SetAmount(RemainRounds);
	}
	//Idle�ִϸ��̼����� ���� : Empty Idle�� ���������� ������ �ϰ� ���� Idle�� ���ư�����
	SetAnimIdle();
}

void UWeaponComponent::SetInfinityAmmo_Implementation(ACPP_WeaponInstance* _Weapon)
{
	AmmoInstance = Cast<ACPP_Ammo>(ACPP_Item::CreateItem(_Weapon->GetAmmoName(),GetWorld(),999));
	check(AmmoInstance);
	bIsInfinityMode = true;
}

void UWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();
	UnarmedWeaponInstance = Cast<ACPP_WeaponInstance>(ACPP_Item::CreateItem(TEXT("Weapon_Unarmed"),GetWorld(),1));
}

void UWeaponComponent::Reload_RPC_Implementation()
{
	Reload_Inner();
}

void UWeaponComponent::InitTPAnim()
{
	//3��Ī �ִϸ��̼� ToDo : 3��Ī �ִϸ��̼��� ��� ���� �ε�

	//Pistol
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso1_ADSShoot.AM_Torso1_ADSShoot"));
		TP_MontageMap.Add(TEXT("ADSShoot_Pistol"), Data.Object);
	}
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso1_HipShoot.AM_Torso1_HipShoot"));
		TP_MontageMap.Add(TEXT("HipShoot_Pistol"), Data.Object);
	}

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso1_Reload.AM_Torso1_Reload"));
		TP_MontageMap.Add(TEXT("Reload_Pistol"), Data.Object);
	}
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso1_CrouchReload.AM_Torso1_CrouchReload"));
		TP_MontageMap.Add(TEXT("Crouch_Reload_Pistol"), Data.Object);
	}

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso1_CrouchShoot.AM_Torso1_CrouchShoot"));
		TP_MontageMap.Add(TEXT("Crouch_Shoot_Pistol"), Data.Object);
	}

	//AssaultRifle
	static auto TPADSShootData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_ADSShoot.AM_Torso2_ADSShoot"));
	TP_MontageMap.Add(TEXT("ADSShoot_AssaultRilfe"), TPADSShootData.Object);

	static auto TPHipShootData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_HipShoot.AM_Torso2_HipShoot"));
	TP_MontageMap.Add(TEXT("HipShoot_AssaultRilfe"), TPHipShootData.Object);

	static auto ReloadData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_Reload.AM_Torso2_Reload"));
	TP_MontageMap.Add(TEXT("Reload_AssaultRilfe"), ReloadData.Object);

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_CrouchReload.AM_Torso2_CrouchReload"));
		TP_MontageMap.Add(TEXT("Crouch_Reload_AssaultRilfe"), Data.Object);
	}

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_CrouchShoot.AM_Torso2_CrouchShoot"));
		TP_MontageMap.Add(TEXT("Crouch_Shoot_AssaultRilfe"), Data.Object);
	}
}

