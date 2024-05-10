// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "Abilities\Tasks\AbilityTask_WaitDelay.h"
#include "..\CPP_PlayableCharacter.h"
#include "CPP_GA_Crouch.h"
UCPP_GA_Sprint::UCPP_GA_Sprint()
{	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//bReplicateInputDirectly 는 InputReleased , InputPressed 함수를 사용할 때 true로 해야 함
	bReplicateInputDirectly = true;
	InputID = EPGAbilityInputID::Sprint;
}

void UCPP_GA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	K2_EndAbility();
}

bool UCPP_GA_Sprint::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/)
{
	bool ReturnValue = Super::CommitCheck(Handle, ActorInfo, ActivationInfo, nullptr);

	//앉은 상태에서 달리기 처리
	FGameplayAbilitySpec* CrouchAbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromInputID(static_cast<int32>(EPGAbilityInputID::Crouch));
	if (CrouchAbilitySpec->IsActive())
	{
		UCPP_GA_Crouch* CrouchAbility = Cast<UCPP_GA_Crouch>(CrouchAbilitySpec->GetAbilityInstances()[0]);
		CrouchAbility->RequestUnCrouchWhenActivateSprint();
		ReturnValue = false;
	}

	return ReturnValue;
}

void UCPP_GA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(GE_Sprint);
	SprintHandle =  CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);

	UAbilityTask_WaitDelay* Delay = UAbilityTask_WaitDelay::WaitDelay(this,0.05f);
	Delay->OnFinish.AddDynamic(this,&UCPP_GA_Sprint::TimerTick);
	Delay->ReadyForActivation();
}

void UCPP_GA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (SprintHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(SprintHandle);
	}
}

void UCPP_GA_Sprint::TimerTick()
{
	ACPP_PlayableCharacter* Character =Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	if (Character->MoveRightAxis == 0 && Character->MoveForwardAxis > 0.98f)
	{
		if (K2_CommitAbilityCost())
		{
			UAbilityTask_WaitDelay* Delay = UAbilityTask_WaitDelay::WaitDelay(this, 0.05f);
			Delay->OnFinish.AddDynamic(this, &UCPP_GA_Sprint::TimerTick);
			Delay->ReadyForActivation();
		}
		else
		{
			K2_EndAbility();
			return;
		}
	}
	else
	{
		K2_EndAbility();
		return;
	}
	
}

void UCPP_GA_Sprint::OnCrouchCheck()
{
	K2_ActivateAbility();
}
