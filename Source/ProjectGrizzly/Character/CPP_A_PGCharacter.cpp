// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_A_PGCharacter.h"

#include <Catch2/catch.hpp>

#include "CPP_PlayableCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFrameWork\CharacterMovementComponent.h"
#include "ProjectGrizzly/Weapon/WeaponComponent.h"

UCPP_A_PGCharacter::UCPP_A_PGCharacter()
{
	//--------------------------------------------------------------------------------------------------
	//										Unarmed
	//--------------------------------------------------------------------------------------------------
	GrizzlyAnimSequence No;
	//HipIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_0_aim_0.StalkerAnim_root_norm_torso_0_aim_0"));
		No.Hip_IdleSequence = Data.Object;
	}
	//HipForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_0_aim_2.StalkerAnim_root_norm_torso_0_aim_2"));
		No.Hip_ForwardSequence = Data.Object;
	}
	//Run
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_0_aim_3.StalkerAnim_root_norm_torso_0_aim_3"));
		No.Hip_RunSequence = Data.Object;
	}
	//HipBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_0_aim_2_back.StalkerAnim_root_norm_torso_0_aim_2_back"));
		No.Hip_BackwardSequence = Data.Object;
	}
	//ADSIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_0_aim_0.StalkerAnim_root_norm_torso_0_aim_0"));
		No.ADS_IdleSequence = Data.Object;
	}
	//ADSForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_0_aim_2.StalkerAnim_root_norm_torso_0_aim_2"));
		No.ADS_ForwardSequence = Data.Object;
	}
	//ADSBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_0_aim_2_back.StalkerAnim_root_norm_torso_0_aim_2_back"));
		No.ADS_BackwardSequence = Data.Object;
	}
	//CrouchIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_torso_0_aim_0.StalkerAnim_root_cr_torso_0_aim_0"));
		No.Crouch_IdleSequence = Data.Object;
	}
	//CrouchForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_torso_0_aim_2.StalkerAnim_root_cr_torso_0_aim_2"));
		No.Crouch_ForwardSequence = Data.Object;
	}
	//CrouchBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_torso_0_aim_2_back.StalkerAnim_root_cr_torso_0_aim_2_back"));
		No.Crouch_BackwardSequence = Data.Object;
	}
	AnimSequenceMap.Add(EWeaponType::No,No);
	
	//Pistol 
	GrizzlyAnimSequence Pistol;
	//HipIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_1_aim_1.StalkerAnim_root_norm_torso_1_aim_1"));
		Pistol.Hip_IdleSequence = Data.Object;
	}
	//HipForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_1_aim_2.StalkerAnim_root_norm_torso_1_aim_2"));
		Pistol.Hip_ForwardSequence = Data.Object;
	}
	//Run
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_1_run_0.StalkerAnim_root_norm_torso_1_run_0"));
		Pistol.Hip_RunSequence = Data.Object;
	}
	//HipBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_1_aim_2_back.StalkerAnim_root_norm_torso_1_aim_2_back"));
		Pistol.Hip_BackwardSequence = Data.Object;
	}
	//ADSIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_2_aim_0.StalkerAnim_root_norm_torso_2_aim_0"));
		Pistol.ADS_IdleSequence = Data.Object;
	}
	//ADSForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_walk_fwd_0.StalkerAnim_root_norm_walk_fwd_0"));
		Pistol.ADS_ForwardSequence = Data.Object;
	}
	//ADSBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_walk_back_0.StalkerAnim_root_norm_walk_back_0"));
		Pistol.ADS_BackwardSequence = Data.Object;
	}
	//CrouchIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_torso_1_aim_0.StalkerAnim_root_cr_torso_1_aim_0"));
		Pistol.Crouch_IdleSequence = Data.Object;
	}
	//CrouchForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_torso_1_aim_2.StalkerAnim_root_cr_torso_1_aim_2"));
		Pistol.Crouch_ForwardSequence = Data.Object;
	}
	//CrouchBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_torso_1_aim_2_back.StalkerAnim_root_cr_torso_1_aim_2_back"));
		Pistol.Crouch_BackwardSequence = Data.Object;
	}
	AnimSequenceMap.Add(EWeaponType::Pistol,Pistol);

	//AssaultRifle
		GrizzlyAnimSequence AssaultRifle;
	//HipIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("AnimSequence'/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_2_aim_1.StalkerAnim_root_norm_torso_2_aim_1'"));
		AssaultRifle.Hip_IdleSequence = Data.Object;
	}
	//HipForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_2_aim_5.StalkerAnim_root_norm_torso_2_aim_5"));
		AssaultRifle.Hip_ForwardSequence = Data.Object;
	}
	//Run
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_2_run_0.StalkerAnim_root_norm_torso_2_run_0"));
		AssaultRifle.Hip_RunSequence = Data.Object;
	}
	//HipBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_2_aim_backward.StalkerAnim_root_norm_torso_2_aim_backward"));
		AssaultRifle.Hip_BackwardSequence = Data.Object;
	}
	//ADSIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_torso_2_aim_0.StalkerAnim_root_norm_torso_2_aim_0"));
		AssaultRifle.ADS_IdleSequence = Data.Object;
	}
	//ADSForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_walk_fwd_0.StalkerAnim_root_norm_walk_fwd_0"));
		AssaultRifle.ADS_ForwardSequence = Data.Object;
	}
	//ADSBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_norm_walk_back_0.StalkerAnim_root_norm_walk_back_0"));
		AssaultRifle.ADS_BackwardSequence = Data.Object;
	}
	//CrouchIdle
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_idle_0.StalkerAnim_root_cr_idle_0"));
		AssaultRifle.Crouch_IdleSequence = Data.Object;
	}
	//CrouchForward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_torso_2_aim_2.StalkerAnim_root_cr_torso_2_aim_2"));
		AssaultRifle.Crouch_ForwardSequence = Data.Object;
	}
	//CrouchBackward
	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimSequenceBase>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/StalkerAnim_root_cr_walk_back_0_.StalkerAnim_root_cr_walk_back_0_"));
		AssaultRifle.Crouch_BackwardSequence = Data.Object;
	}
	AnimSequenceMap.Add(EWeaponType::AssaultRilfe,AssaultRifle);
}

void UCPP_A_PGCharacter::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(TryGetPawnOwner());
	if (!IsValid(Character))
	{
		return;
	}
	//현재 무기에 맞춘 3인칭 애니메이션 출력
	if(!IsValid(Character->GetWeaponComponent()))
		return;
	if(Character->GetWeaponComponent()->GetCurrentWeaponData() == nullptr)
		return;
	EWeaponType WeaponType = Character->GetWeaponComponent()->GetWeaponType();
	if(CurrentWeaponType != WeaponType)
	{
		CurrentWeaponType = WeaponType;
		SetAnimMode(WeaponType);
	}

	//AI는 속도로 MoveForwardAxis를 업데이트
	if (Character->bIsAI)
	{
		FVector Velocity = Character->GetMovementComponent()->Velocity;
		Velocity.Z = 0;
		if (Velocity.Length() > 0)
		{
			MoveForwardAxis = 1.f;
		}
		else
		{
			MoveForwardAxis = 0.f;
		}
	}
	else
	{
		MoveForwardAxis = Character->MoveForwardAxis;
	}
	MoveRightAxis = Character->MoveRightAxis;
	BendDownDegree = Character->BendDownDegree;

	UpdateAxis(DeltaSeconds);
	UpdateADS();
	UpdateLeftHandIK();
	UpdateDead();
	UpdateSprint(Character);
	UpdateLeaning(DeltaSeconds);
	bIsCrouching = Character->GetCharacterMovement()->IsCrouching();
}

void UCPP_A_PGCharacter::SetAnimMode(EWeaponType _WeaponType)
{
	GrizzlyAnimSequence AnimSequences = AnimSequenceMap[_WeaponType];

	Hip_IdleSequence = AnimSequences.Hip_IdleSequence;
	Hip_ForwardSequence = AnimSequences.Hip_ForwardSequence;
	Hip_RunSequence = AnimSequences.Hip_RunSequence;
	Hip_BackwardSequence = AnimSequences.Hip_BackwardSequence;
	ADS_IdleSequence = AnimSequences.ADS_IdleSequence;
	ADS_ForwardSequence = AnimSequences.ADS_ForwardSequence;
	ADS_BackwardSequence = AnimSequences.ADS_BackwardSequence;
	Crouch_IdleSequence = AnimSequences.Crouch_IdleSequence;
	Crouch_ForwardSequence = AnimSequences.Crouch_ForwardSequence;
	Crouch_BackwardSequence = AnimSequences.Crouch_BackwardSequence;
}

void UCPP_A_PGCharacter::UpdateSprint(ACPP_PlayableCharacter* Character)
{
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Sprint"))))
		{
			bSprint = true;
		}
		else
		{
			bSprint = false;
		}
	}
}


void UCPP_A_PGCharacter::AnimNotify_OnReloadEnd()
{
	LeftHandIKAlpha = 1.f;
}

void UCPP_A_PGCharacter::AnimNotify_OnReloadStart()
{
	LeftHandIKAlpha = 0.f;
	if (OnReloadStartDelegate.IsBound())
	{
		OnReloadStartDelegate.Broadcast();
	}

}

void UCPP_A_PGCharacter::UpdateAxis(float _DeltaSeconds)
{
	if (MoveForwardAxis == 0.f && MoveRightAxis != 0)
	{
		MoveForwardAxis = 1.f;
	}

	float DefaultRightRotation = 40.f;
	float RightRotation = DefaultRightRotation * MoveRightAxis * MoveForwardAxis;

	RightRotationInterpo = FMath::FInterpTo(RightRotationInterpo, RightRotation, _DeltaSeconds, 10.f);
}

void UCPP_A_PGCharacter::UpdateADS()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(TryGetPawnOwner());
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return;
	}
	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS"))) && !ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADSRelieve"))))
	{
		bADS = true;
	}
	else
	{
		bADS = false;
	}
}

void UCPP_A_PGCharacter::UpdateLeftHandIK()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(TryGetPawnOwner());
	USkeletalMeshComponent* TPWeapon = Character->GetTPWeaponComponent();
	if (!IsValid(TPWeapon))
	{
		return;
	}
	FTransform WeaponLeftHandTransform = TPWeapon->GetSocketTransform(TEXT("LeftHand"));
	FVector OutputVector; 
	FRotator OutputRotator;

	Character->GetMesh()->TransformToBoneSpace(TEXT("bip01_r_hand"), WeaponLeftHandTransform.GetLocation(), WeaponLeftHandTransform.GetRotation().Rotator(), OutputVector, OutputRotator);
	LeftHandTransform.SetLocation(OutputVector);
	LeftHandTransform.SetRotation(OutputRotator.Quaternion());

}

void UCPP_A_PGCharacter::UpdateLeaning(float _DeltaSeconds)
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(TryGetPawnOwner());
	float LeaningSpeed = 10.0f;
	LeaningRate = FMath::Lerp(LeaningRate, Character->GetLeaningAxis(), _DeltaSeconds * LeaningSpeed);
}


void UCPP_A_PGCharacter::UpdateDead()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(TryGetPawnOwner());
	bIsDead = Character->IsDead();
}
