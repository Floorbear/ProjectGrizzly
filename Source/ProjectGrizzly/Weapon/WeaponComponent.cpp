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
	// ensure(WeaponData != NULL); // WeaponName 에 해당하는 데이터가 존재하지 않음
	//
	// WeaponAnim = GetWeaponAnimTable()->FindRow<FWeaponAnim>(WeaponName, FString(""));
	// ensure(WeaponData != NULL); // WeaponName 에 해당하는 데이터가 존재하지 않음

	//SetWeapon(TEXT("AKM"));

}


void UWeaponComponent::SetWeapon_Implementation(ACPP_WeaponInstance* _WeaponInstance)
{
	CurrentWeaponInstance = _WeaponInstance;
	FWeaponData* _WeaponData = _WeaponInstance->GetWeaponData();
	checkf(_WeaponData != nullptr,TEXT("Cant Find %s in WeaponDT"),*_WeaponInstance->GetWeaponData()->WeaponName);
	// 무기 반동 데이터
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
	//총 그림자
	Character->GetShadowWeaponComponent()->SetSkeletalMesh(WeaponMesh);
	//1인칭 매쉬
	Character->GetFPWeaponMeshComponent()->SetSkeletalMesh(WeaponMesh);
	Character->GetFPWeaponMeshComponent()->SetAnimInstanceClass(_WeaponData->WeaponAnimClass);
	//비무장 상태 예외처리
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
	//반동
	WeaponRecoilCurve = LoadObject<UCurveVector>(NULL, *_WeaponData->WeaponRecoilCurve.ToSoftObjectPath().ToString());

	//무기 발사모드는 무기 인스턴스에 옮김
	

	//--------------------------------------------------------------------------------------------------------------
	//											Animation
	//--------------------------------------------------------------------------------------------------------------
	FWeaponAnim* WeaponAnim = GetWeaponAnimDT()->FindRow<FWeaponAnim>(CurrentWeaponName,FString(""));
	check(WeaponAnim != nullptr);
	
	//1인칭 오른손 소캣 모드
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
	
	//3인칭 왼손 IK
	UCPP_A_PGCharacter* AnimInstance = Cast<UCPP_A_PGCharacter>(Character->GetMesh()->GetAnimInstance());
	if(_WeaponData->WeaponType == EWeaponType::AssaultRilfe)
		AnimInstance->LeftHandIKAlpha = 1.f;
	else
		AnimInstance->LeftHandIKAlpha = 0.f;

	
	UAnimMontage* WeaponShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_Shoot.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(TEXT("Shoot"), WeaponShoot);
	UAnimMontage* HandsShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Hands_Shoot.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Shoot"), HandsShoot);

	//총의 Idle이 Empty면 변하는 애니메이션이 존재하면 로딩
	if(HasEmptyIdleAnim())
	{
		LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Weapon_EmptyIdle.ToSoftObjectPath().ToString());
		if(IsMagazineEmpty())
		{
			SetEmptyIdleAnim();
		}
	}
	
	//마지막 한발 애니메이션이 있으면 추가
	if(!WeaponAnim->Weapon_LastShoot.IsNull())
	{
		UAnimMontage* WeaponLastShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_LastShoot.ToSoftObjectPath().ToString());
		FP_Weapon_MontageMap.Add(TEXT("Shoot_Last"), WeaponLastShoot);
	}
	UAnimMontage* WeaponLastShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_Shoot.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(TEXT("Shoot"), WeaponShoot);



	LoadFPAnim(WeaponAnim->Weapon_Reload, WeaponAnim->Hands_Reload, TEXT("Reload"));
	LoadFPAnim(WeaponAnim->Weapon_TacReload, WeaponAnim->Hands_TacReload, TEXT("TacReload"));
	//드로우 & 언드로우 애니메이션
	UAnimMontage* Draw = LoadObject<UAnimMontage>(NULL, *WeaponAnim->AM_Hands_Draw.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Draw"), Draw);
	UAnimMontage* Undraw = LoadObject<UAnimMontage>(NULL, *WeaponAnim->AM_Hands_Undraw.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Undraw"), Undraw);

	//손 위치 & 회전
	Character->SetIdleHandsLocation(_WeaponData->IdleHandsLocation);
	Character->SetADSHandsLocation(_WeaponData->ADSHandsLocation);
	Character->SetHandsRotation(_WeaponData->HandsRotation);

	//손 애니메이션 초기화
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
	
	//총기의 재생 애니메이션이 Shoot 일경우 만약 LastShoot 모션이 존재하고 마지막 한발을 발사하는 상황이면
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
	ensure(ReturnValue != NULL); // _Name에 해당하는 애니메이션이 Map에 존재하지 않음
	return ReturnValue;
}

UAnimMontage* UWeaponComponent::Get_FP_Hands_Montage(FName _Name) const
{
	UAnimMontage* ReturnValue = *FP_Hands_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name에 해당하는 애니메이션이 Map에 존재하지 않음
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
	ensure(ReturnValue != NULL); // _Name에 해당하는 애니메이션이 Map에 존재하지 않음
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
	//총알이 없을때 Empty 애니메이션이 있다면 변경
	if(IsMagazineEmpty() && HasEmptyIdleAnim() && GetOwner()->HasAuthority())
	{
		SetEmptyIdleAnim();
	}
}

void UWeaponComponent::Reload_Inner()
{
	//총알이 없으면 재장전 No
	if(AmmoInstance == nullptr)
	{
		return;
	}
	int RemainRounds = GetRemainRounds();
	int CurrentMagazineRounds = GetCurrentMagazineRounds();

	//택티컬 리로딩
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

	//무기 인스턴스에 총알 설정
	CurrentWeaponInstance->SetRounds(CurrentMagazineRounds);
	
	if(!bIsInfinityMode)
	{
		AmmoInstance->SetAmount(RemainRounds);
	}
	//Idle애니메이션으로 변경 : Empty Idle을 사용했을경우 재장전 하고 정상 Idle로 돌아가야함
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
	//3인칭 애니메이션 ToDo : 3인칭 애니메이션은 모두 동기 로딩

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

