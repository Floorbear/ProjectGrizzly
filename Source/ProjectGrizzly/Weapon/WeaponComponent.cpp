// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "CPP_WeaponAnimInstance.h"
#include "..\Character\CPP_PlayableCharacter.h"
#include "..\Character\CPP_PGCharacter.h"
#include "..\Character\CPP_A_Hands.h"
#include "Net\UnrealNetwork.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//ToDo : DT to GameInstance
	static auto DataTable =  ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponDataTable.WeaponDataTable"));
	WeaponDataTable = DataTable.Object;

	static auto AnimDataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponAnimTable.WeaponAnimTable"));
	WeaponAnimTable = AnimDataTable.Object;


	//3인칭 애니메이션 ToDo : 3인칭 애니메이션은 모두 동기 로딩
	static auto TPADSShootData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_ADSShoot.AM_Torso2_ADSShoot"));
	TP_MontageMap.Add(TEXT("ADSShoot"), TPADSShootData.Object);

	static auto TPHipShootData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_HipShoot.AM_Torso2_HipShoot"));
	TP_MontageMap.Add(TEXT("HipShoot"), TPHipShootData.Object);

	static auto ReloadData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_Reload.AM_Torso2_Reload"));
	TP_MontageMap.Add(TEXT("Reload"), ReloadData.Object);

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_CrouchReload.AM_Torso2_CrouchReload"));
		TP_MontageMap.Add(TEXT("Crouch_Reload"), Data.Object);
	}

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_CrouchShoot.AM_Torso2_CrouchShoot"));
		TP_MontageMap.Add(TEXT("Crouch_Shoot"), Data.Object);
	}

	//컴포넌트의 레플리케이션 설정은 액터에서 한다


}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	WeaponData = GetWeaponDataTable()->FindRow<FWeaponData>(WeaponName, FString(""));
	ensure(WeaponData != NULL); // WeaponName 에 해당하는 데이터가 존재하지 않음

	WeaponAnim = GetWeaponAnimTable()->FindRow<FWeaponAnim>(WeaponName, FString(""));
	ensure(WeaponData != NULL); // WeaponName 에 해당하는 데이터가 존재하지 않음

	InitWeaponData();

}


void UWeaponComponent::InitWeaponData()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	ensure(Character != NULL);
	USkeletalMesh* WeaponMesh = LoadObject<USkeletalMesh>(NULL, *WeaponData->WeaponSkeletalMesh.ToSoftObjectPath().ToString());
	Character->GetTPWeaponComponent()->SetSkeletalMesh(WeaponMesh);
	//총 그림자
	Character->GetShadowWeaponComponent()->SetSkeletalMesh(WeaponMesh);

	//탄창
	PerMagazineRounds = WeaponData->Rounds;
	RemainRounds = PerMagazineRounds * 5;
	CurrentMagazineRounds = PerMagazineRounds;

	Character->GetFPWeaponMeshComponent()->SetSkeletalMesh(WeaponMesh);
	Character->GetFPWeaponMeshComponent()->SetAnimInstanceClass(WeaponData->WeaponAnimClass);

	//무기 발사 모드
	CurrentWeaponMode = WeaponData->WeaponModes[0];

	// 무기 반동 데이터
	WeaponRecoilCurve = LoadObject<UCurveVector>(NULL, *WeaponData->WeaponRecoilCurve.ToSoftObjectPath().ToString());

	//Empty 애니메이션이 존재하면 로딩
	if(!WeaponAnim->Weapon_EmptyIdle.IsNull())
	{
		LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_EmptyIdle.ToSoftObjectPath().ToString());
	}
	
	//애니메이션
	UAnimMontage* WeaponShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_Shoot.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(TEXT("Shoot"), WeaponShoot);
	UAnimMontage* HandsShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Hands_Shoot.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Shoot"), HandsShoot);

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
	Character->SetIdleHandsLocation(WeaponData->IdleHandsLocation);
	Character->SetADSHandsLocation(WeaponData->ADSHandsLocation);
	Character->SetHandsRotation(WeaponData->HandsRotation);

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
	UAnimMontage* ReturnValue = *TP_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name에 해당하는 애니메이션이 Map에 존재하지 않음
	return ReturnValue;
}

class UDataTable* UWeaponComponent::GetWeaponDataTable() const
{
	check(WeaponDataTable != NULL)
	return WeaponDataTable;
}

class UDataTable* UWeaponComponent::GetWeaponAnimTable() const
{
	check(WeaponAnimTable != NULL)
	return WeaponAnimTable;
}

void UWeaponComponent::SetCurrentMagazineRounds(int _NewValue)
{
	if (GetOwner()->HasAuthority())
	{
		CurrentMagazineRounds = FMath::Clamp(_NewValue, 0, PerMagazineRounds + 1);

		//총알이 없을때 Empty 애니메이션이 있다면 변경
		if(CurrentMagazineRounds == 0 && !WeaponAnim->Weapon_EmptyIdle.IsNull())
		{
			ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
			UCPP_WeaponAnimInstance* AnimInstance = Cast<UCPP_WeaponAnimInstance>(Character->GetFPWeaponMeshComponent()->GetAnimInstance());
			UAnimSequence* EmptyAnimation = Cast<UAnimSequence>(WeaponAnim->Weapon_EmptyIdle.Get());
			AnimInstance->SetIdle(EmptyAnimation);
		}
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
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	UCPP_WeaponAnimInstance* AnimInstance = Cast<UCPP_WeaponAnimInstance>(Character->GetFPWeaponMeshComponent()->GetAnimInstance());
	UAnimSequence* IdleAnimation = Cast<UAnimSequence>(WeaponAnim->Weapon_Idle.Get());
	AnimInstance->SetIdle(IdleAnimation);
}

void UWeaponComponent::Reload_RPC_Implementation()
{
	Reload_Inner();
}

