// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_A_Hands.h"

UCPP_A_Hands::UCPP_A_Hands()
{

}

void UCPP_A_Hands::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!IsValid(GetPlayer()))
	{
		return;
	}

	MoveForwardAxis = GetPlayer()->MoveForwardAxis;
	MoveRightAxis = GetPlayer()->MoveRightAxis;
	//Not Replicated
	MouseXDelta = GetPlayer()->MouseXDelta;
	MouseYDelta = GetPlayer()->MouseYDelta;

	//Hand Sway 
	float RollDelta =  FMath::GetMappedRangeValueClamped(TRange<float>(1.f, -1.f), TRange<float>(-4 * HandSwayYFactorDuringAiming, 4 * HandSwayYFactorDuringAiming),MouseYDelta);
	HandSwayRotator.Roll = FMath::FInterpTo(HandSwayRotator.Roll, RollDelta, DeltaSeconds, 9.f);

	float YawDelta = FMath::GetMappedRangeValueClamped(TRange<float>(1.f, -1.f), TRange<float>(-10 * HandSwayXFactorDuringAiming, 10 * HandSwayXFactorDuringAiming), MouseXDelta);
	HandSwayRotator.Yaw = FMath::FInterpTo(HandSwayRotator.Yaw, YawDelta, DeltaSeconds, 6.f);

	UpdateADS();
	UpdateSprint();
}

void UCPP_A_Hands::UpdateSprint()
{
	ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(TryGetPawnOwner());
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

void UCPP_A_Hands::UpdateADS()
{
	ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(TryGetPawnOwner());
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS"))) && !ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADSRelieve"))))
		{
			bADS = true;
		}
		else
		{
			bADS = false;
		}
	}
}

class ACPP_PlayableCharacter* UCPP_A_Hands::GetPlayer()
{
	if (!Player.IsValid())
	{
		Player = static_cast<ACPP_PlayableCharacter*>(TryGetPawnOwner());
	}
	return Player.Get();
}
