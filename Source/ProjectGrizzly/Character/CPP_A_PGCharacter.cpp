// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_A_PGCharacter.h"
#include "CPP_PGCharacter.h"
#include "AbilitySystemComponent.h"

void UCPP_A_PGCharacter::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(TryGetPawnOwner());
	if (!IsValid(Character))
	{
		return;
	}

	MoveForwardAxis = Character->MoveForwardAxis;
	MoveRightAxis = Character->MoveRightAxis;
	BendDownDegree = Character->BendDownDegree;

	UpdateAxis(DeltaSeconds);
	UpdateADS();
	UpdateLeftHandIK();
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
	ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(TryGetPawnOwner());
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
	ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(TryGetPawnOwner());
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
