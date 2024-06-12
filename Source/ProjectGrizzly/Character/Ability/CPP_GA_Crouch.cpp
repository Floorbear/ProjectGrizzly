// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Crouch.h"
#include "..\CPP_PlayableCharacter.h"
#include "Abilities\Tasks\AbilityTask_WaitInputPress.h"
#include "Abilities\Tasks\AbilityTask_WaitDelay.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Abilities\Tasks\AbilityTask_WaitGameplayEvent.h"
UCPP_GA_Crouch::UCPP_GA_Crouch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::Crouch;
	bReplicateInputDirectly = true;
}

bool UCPP_GA_Crouch::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/)
{
	bool ReturnValue = Super::CommitCheck(Handle, ActorInfo, ActivationInfo, nullptr);
	if (!GetCharacter()->CanCrouch())
	{
		ReturnValue = false;
	}
	return ReturnValue;
}

void UCPP_GA_Crouch::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (CanBeCanceled())
	{
		RequestUnCrouch();

	}
}

void UCPP_GA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetCharacter()->Crouch();

	UAbilityTask_WaitInputPress* ToggleCrouch = UAbilityTask_WaitInputPress::WaitInputPress(this);
	ToggleCrouch->OnPress.AddDynamic(this,&UCPP_GA_Crouch::OnPressed);
	ToggleCrouch->ReadyForActivation();

	//AI Crouch 해제용 콜백 
	auto AIUnCrouchListener = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UnCrouch")));
	AIUnCrouchListener->EventReceived.AddDynamic(this,&UCPP_GA_Crouch::OnReceiveUnCrouchEvent);
	AIUnCrouchListener->ReadyForActivation();
}

void UCPP_GA_Crouch::OnPressed(float _TimeWaited)
{
	RequestUnCrouch();

	UAbilityTask_WaitInputPress* ToggleCrouch = UAbilityTask_WaitInputPress::WaitInputPress(this);
	ToggleCrouch->OnPress.AddDynamic(this, &UCPP_GA_Crouch::OnPressed);
	ToggleCrouch->ReadyForActivation();
}

void UCPP_GA_Crouch::OnFinish()
{
	if (GetCharacter()->bIsCrouched == false)
	{
		K2_EndAbility();
	}
	else
	{
		GetCharacter()->GetCharacterMovement()->bWantsToCrouch = true;
	}
}

void UCPP_GA_Crouch::OnDelayFinishWhenActivateSprint()
{
	if (GetCharacter()->bIsCrouched == false)
	{
		K2_EndAbility();

		FGameplayTagContainer Container;
		Container.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Sprint")));
		CurrentActorInfo->AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
	}
	else
	{
		GetCharacter()->GetCharacterMovement()->bWantsToCrouch = true;
	}
}

void UCPP_GA_Crouch::RequestUnCrouch()
{
	GetCharacter()->UnCrouch();
	UAbilityTask_WaitDelay* CheckUncrouch = UAbilityTask_WaitDelay::WaitDelay(this, 0.05f);
	CheckUncrouch->OnFinish.AddDynamic(this, &UCPP_GA_Crouch::OnFinish);
	CheckUncrouch->ReadyForActivation();
}

void UCPP_GA_Crouch::RequestUnCrouchWhenActivateSprint()
{
	GetCharacter()->UnCrouch();
	UAbilityTask_WaitDelay* CheckUncrouch = UAbilityTask_WaitDelay::WaitDelay(this, 0.05f);
	CheckUncrouch->OnFinish.AddDynamic(this, &UCPP_GA_Crouch::OnDelayFinishWhenActivateSprint);
	CheckUncrouch->ReadyForActivation();
}

void UCPP_GA_Crouch::OnReceiveUnCrouchEvent(FGameplayEventData Payload)
{
	RequestUnCrouch();
}
