// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_A_PGCharacter.h"
#include "CPP_PGCharacter.h"

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
}

void UCPP_A_PGCharacter::AnimNotify_OnReloadStart()
{
	if (OnReloadStartDelegate.IsBound())
	{
		OnReloadStartDelegate.Broadcast();
	}

}
