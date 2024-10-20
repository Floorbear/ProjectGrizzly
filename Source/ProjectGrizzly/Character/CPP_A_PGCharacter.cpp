// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_A_PGCharacter.h"
#include "CPP_PlayableCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFrameWork\CharacterMovementComponent.h"
void UCPP_A_PGCharacter::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(TryGetPawnOwner());
	if (!IsValid(Character))
	{
		return;
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
