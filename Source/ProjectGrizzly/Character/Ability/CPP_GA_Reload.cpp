// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Reload.h"
#include "Abilities\Tasks\AbilityTask.h"
#include "Abilities\Tasks\AbilityTask_PlayMontageAndWait.h"

UCPP_GA_Reload::UCPP_GA_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::Reload;
}

void UCPP_GA_Reload::OnBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true,true);
}

void UCPP_GA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true,true);
	}

	//ToDo : 전술 재장전, 재장전 분리
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ReloadMontage);
	Task->OnBlendOut.AddDynamic(this, &UCPP_GA_Reload::OnBlendOut);
	Task->ReadyForActivation();
	UE_LOG(LogTemp, Warning, TEXT("Reload On"));

	// Task의 Tick https://dev.epicgames.com/community/learning/tutorials/qZYY/unreal-engine-on-tick-ability-task
}


