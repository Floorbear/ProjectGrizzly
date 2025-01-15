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
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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

	SetWeapon(TEXT("AKM"));

}


void UWeaponComponent::SetWeapon(FName _WeaponName, bool bIsPrimary)
{
	WeaponName = _WeaponName;
	FWeaponData* _WeaponData = Cast<UGrizzlyGameInstance>(GetWorld()->GetGameInstance())->GetWeaponDataTable()->FindRow<FWeaponData>(_WeaponName, FString(""));
	checkf(_WeaponData != nullptr,TEXT("Cant Find %s in WeaponDT"),_WeaponName);
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	ensure(Character != NULL);
	USkeletalMesh* WeaponMesh = LoadObject<USkeletalMesh>(NULL, *_WeaponData->WeaponSkeletalMesh.ToSoftObjectPath().ToString());
	Character->GetTPWeaponComponent()->SetSkeletalMesh(WeaponMesh);
	//총 그림자d
	Character->GetShadowWeaponComponent()->SetSkeletalMesh(WeaponMesh);

	//탄창
	PerMagazineRounds = _WeaponData->Rounds;
	RemainRounds = PerMagazineRounds * 5;
	CurrentMagazineRounds = PerMagazineRounds;

	Character->GetFPWeaponMeshComponent()->SetSkeletalMesh(WeaponMesh);
	Character->GetFPWeaponMeshComponent()->SetAnimInstanceClass(_WeaponData->WeaponAnimClass);

	//무기 발사 모드
	CurrentWeaponMode = _WeaponData->WeaponModes[0];

	// 무기 반동 데이터
	WeaponRecoilCurve = LoadObject<UCurveVector>(NULL, *_WeaponData->WeaponRecoilCurve.ToSoftObjectPath().ToString());


	

	//--------------------------------------------------------------------------------------------------------------
	//											Animation
	//--------------------------------------------------------------------------------------------------------------
	FWeaponAnim* WeaponAnim = GetWeaponAnimTable()->FindRow<FWeaponAnim>(_WeaponName,FString(""));
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
	if(_WeaponData->WeaponType == EWeaponType::Pistol)
		AnimInstance->LeftHandIKAlpha = 0.f;
	else
		AnimInstance->LeftHandIKAlpha = 1.f;
		
	UAnimMontage* WeaponShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_Shoot.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(TEXT("Shoot"), WeaponShoot);
	UAnimMontage* HandsShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Hands_Shoot.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Shoot"), HandsShoot);

	//Empty 애니메이션이 존재하면 로딩
	if(!WeaponAnim->Weapon_EmptyIdle.IsNull())
	{
		LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_EmptyIdle.ToSoftObjectPath().ToString());
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

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FWeaponData* UWeaponComponent::GetCurrentWeaponData() const
{
	FWeaponData* Data = GetWeaponDataTable()->FindRow<FWeaponData>(WeaponName,FString(""));
	check(Data != nullptr);
	return Data;
}

FWeaponAnim* UWeaponComponent::GetCurrentAnimData() const
{
	FWeaponAnim* Data = GetWeaponAnimTable()->FindRow<FWeaponAnim>(WeaponName,FString(""));
	checkf(Data != nullptr,TEXT("%s is illegal"),WeaponName);
	return Data;
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, CurrentMagazineRounds);
	DOREPLIFETIME(UWeaponComponent, RemainRounds);
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
	
	//_Name == Shoot 일경우 LastShoot 모션이 존재하고 1발 남은 상황이면 그 모션을 재생한다
	if(CurrentMagazineRounds == 0)
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



void UWeaponComponent::SetAnimIdle_Implementation()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	UCPP_WeaponAnimInstance* AnimInstance = Cast<UCPP_WeaponAnimInstance>(Character->GetFPWeaponMeshComponent()->GetAnimInstance());
	UAnimSequence* IdleAnimation = Cast<UAnimSequence>(GetCurrentAnimData()->Weapon_Idle.Get());
	AnimInstance->SetIdle(IdleAnimation);
}

void UWeaponComponent::SetAnimEmptyIdle_Implementation()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	UCPP_WeaponAnimInstance* AnimInstance = Cast<UCPP_WeaponAnimInstance>(Character->GetFPWeaponMeshComponent()->GetAnimInstance());
	UAnimSequence* EmptyAnimation = Cast<UAnimSequence>(GetCurrentAnimData()->Weapon_EmptyIdle.Get());
	AnimInstance->SetIdle(EmptyAnimation);
}

class UDataTable* UWeaponComponent::GetWeaponDataTable() const
{
	static UDataTable* DT =  Cast<UGrizzlyGameInstance>(GetWorld()->GetGameInstance())->GetWeaponDataTable();
	check(DT != NULL)
	return DT;
}

class UDataTable* UWeaponComponent::GetWeaponAnimTable() const
{
	static UDataTable* DT =  Cast<UGrizzlyGameInstance>(GetWorld()->GetGameInstance())->GetWeaponAnimTable();
	check(DT != NULL)
	return DT;
}

void UWeaponComponent::SetCurrentMagazineRounds(int _NewValue)
{
	CurrentMagazineRounds = FMath::Clamp(_NewValue, 0, PerMagazineRounds + 1);

	//총알이 없을때 Empty 애니메이션이 있다면 변경
	if(CurrentMagazineRounds == 0 && !GetCurrentAnimData()->Weapon_EmptyIdle.IsNull() && GetOwner()->HasAuthority())
	{
		SetAnimEmptyIdle();
	}
}

void UWeaponComponent::SetRemainRounds(int _NewValue)
{
	if (GetOwner()->HasAuthority())
	{
		RemainRounds = FMath::Clamp(_NewValue, 0, 999);
	}
}

void UWeaponComponent::Reload_Inner()
{
	if (RemainRounds + CurrentMagazineRounds >= PerMagazineRounds)
	{
		if (CurrentMagazineRounds > 0)
		{
			RemainRounds += CurrentMagazineRounds;
			if (RemainRounds == PerMagazineRounds)
			{
				CurrentMagazineRounds = PerMagazineRounds;
			}
			else
			{
				CurrentMagazineRounds = PerMagazineRounds + 1;
			}
			RemainRounds -= CurrentMagazineRounds;
		}
		else
		{
			RemainRounds += CurrentMagazineRounds;
			CurrentMagazineRounds = PerMagazineRounds;
			RemainRounds -= CurrentMagazineRounds;
		}
	}
	else
	{
		RemainRounds += CurrentMagazineRounds;
		CurrentMagazineRounds = RemainRounds;
		RemainRounds = 0;
	}
	
	//Idle애니메이션으로 변경 : Empty Idle을 사용했을경우 재장전 하고 정상 Idle로 돌아가야함
	SetAnimIdle();

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

