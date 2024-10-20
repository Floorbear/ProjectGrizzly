// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_HighReady.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "..\CPP_Player.h"
UCPP_GA_HighReady::UCPP_GA_HighReady()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.HighReady")));

	//하이레디 중 사격 금지
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Shoot")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Shoot")));

}

void UCPP_GA_HighReady::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	Super::ActivateAbility(Handle,ActorInfo,ActivationInfo,TriggerEventData);

	UAbilityTask_WaitDelay* Timer = UAbilityTask_WaitDelay::WaitDelay(this,0.05f);
	Timer->OnFinish.AddDynamic(this, &UCPP_GA_HighReady::TimerTick);
	Timer->ReadyForActivation();

}

void UCPP_GA_HighReady::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCPP_GA_HighReady::TimerTick()
{
	auto Player = GetCharacter();
	ensure(Player);
	if(!Player->bHighReady)
		K2_EndAbility();
	UAbilityTask_WaitDelay* Timer = UAbilityTask_WaitDelay::WaitDelay(this, 0.05f);
	Timer->OnFinish.AddDynamic(this, &UCPP_GA_HighReady::TimerTick);
	Timer->ReadyForActivation();
}
