// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Shoot.h"
#include "..\CPP_PlayableCharacter.h"
#include "..\CPP_A_PGCharacter.h"
#include "AbilitySystemComponent.h"
#include "Abilities\Tasks\AbilityTask_PlayMontageAndWait.h"
#include "Abilities\Tasks\AbilityTask_WaitGameplayEvent.h"

UCPP_GA_Shoot::UCPP_GA_Shoot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::Shoot;
}

void UCPP_GA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(AM_Hands_Shoot);
	Player->GetFPWeaponMeshComponent()->GetAnimInstance()->Montage_Play(AM_Weapon_Shoot);
	bool bADS = Player->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	UAbilityTask_PlayMontageAndWait* Task = NULL;
	if (bADS)
	{
		Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_ADS_Shoot,1.0f,NAME_None,false);
	}
	else
	{
		Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_Hip_Shoot, 1.0f, NAME_None, false);
	}
	Task->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* EventReceivedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(("Event.Montage.EndAbility")));
	EventReceivedTask->EventReceived.AddDynamic(this, &UCPP_GA_Shoot::EventReceivedd);
	EventReceivedTask->ReadyForActivation();
}

void UCPP_GA_Shoot::EventReceivedd(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
