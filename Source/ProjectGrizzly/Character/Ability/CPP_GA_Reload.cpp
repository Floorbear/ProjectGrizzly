// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Reload.h"
#include "Abilities\Tasks\AbilityTask.h"
#include "Abilities\Tasks\AbilityTask_PlayMontageAndWait.h"
#include "..\CPP_A_PGCharacter.h"
#include "..\CPP_PlayableCharacter.h"

UCPP_GA_Reload::UCPP_GA_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::Reload;
}

void UCPP_GA_Reload::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true,true);
}

void UCPP_GA_Reload::BindReloadMontage()
{
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(GetCurrentHandsReloadMontage());
	Player->GetFPWeaponMeshComponent()->GetAnimInstance()->Montage_Play(GetCurrentWeaponReloadMontage());
}

void UCPP_GA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true,true);
	}

	if (!bIsBindDelegate)
	{
		bIsBindDelegate = true;
		//ToDo : AIController 상황의 분기 추가하기
		ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
		UCPP_A_PGCharacter* AnimInstance = Cast<UCPP_A_PGCharacter>(Player->GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance))
		{
			AnimInstance->OnReloadStartDelegate.AddDynamic(this, &UCPP_GA_Reload::BindReloadMontage);

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Vaild AnimInstance"));

		}
	}
	BP_ActivateAbility();
	//ToDo : 전술 재장전, 재장전 분리
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TPReloadMontage, GetCurrentTPReloadRate());
	Task->OnCompleted.AddDynamic(this, &UCPP_GA_Reload::OnCompleted);
	Task->ReadyForActivation();

	// Task의 Tick https://dev.epicgames.com/community/learning/tutorials/qZYY/unreal-engine-on-tick-ability-task
}

class UAnimMontage* UCPP_GA_Reload::GetCurrentHandsReloadMontage()
{
	//ToDo : Weapon 액터가 구현되면, 거기서 몽타주를 참조할 것
	return ShouldDelete_HandsReloadMontage;
}

class UAnimMontage* UCPP_GA_Reload::GetCurrentWeaponReloadMontage()
{

	return ShouldDelete_WeaponReloadMontage;
}

float UCPP_GA_Reload::GetCurrentTPReloadRate()
{
	return 0.8f;
}

