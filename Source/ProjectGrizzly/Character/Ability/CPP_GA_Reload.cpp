// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Reload.h"
#include "Abilities\Tasks\AbilityTask.h"
#include "Abilities\Tasks\AbilityTask_PlayMontageAndWait.h"
#include "..\..\Weapon\\WeaponComponent.h"
#include "..\CPP_A_PGCharacter.h"
#include "..\CPP_PlayableCharacter.h"
#include "GameFramework\CharacterMovementComponent.h"

UCPP_GA_Reload::UCPP_GA_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::Reload;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Reload")));

	//������ �� ���, ������ ��Ȱ��ȭ
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	//������ �� ����̱� ��Ȱ��ȭ
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Leaning")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Leaning")));

}

bool UCPP_GA_Reload::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/)
{
	bool ReturnValue  = Super::CommitCheck(Handle, ActorInfo, ActivationInfo, nullptr);
	if (!CanReload())
	{
		ReturnValue = false;
	}
	return ReturnValue;
}

void UCPP_GA_Reload::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (CanBeCanceled())
	{
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
		ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);

		Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_StopWithBlendOut(0.1f,GetCurrentHandsReloadMontage());
		Player->GetFPWeaponMeshComponent()->GetAnimInstance()->Montage_StopWithBlendOut(0.1f,GetCurrentWeaponReloadMontage());
	}
	
}

void UCPP_GA_Reload::OnCompleted()
{
	GetWeaponComponent()->Reload();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true,true);
}

void UCPP_GA_Reload::BindReloadMontage()
{
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);

}

void UCPP_GA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true,true);
		return;
	}
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	BP_ActivateAbility();

	bool bIsCrouching = GetCharacter()->GetCharacterMovement()->IsCrouching();
	UAbilityTask_PlayMontageAndWait* Task;
	if (bIsCrouching)
	{
		Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GetWeaponComponent()->Get_TP_Montage(TEXT("Crouch_Reload")), GetCurrentTPReloadRate());

	}
	else
	{
		Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GetWeaponComponent()->Get_TP_Montage(TEXT("Reload")), GetCurrentTPReloadRate());
	}
	Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(GetCurrentHandsReloadMontage());
	Player->GetFPWeaponMeshComponent()->GetAnimInstance()->Montage_Play(GetCurrentWeaponReloadMontage());
	Task->OnCompleted.AddDynamic(this, &UCPP_GA_Reload::OnCompleted);
	Task->ReadyForActivation();



	// Task�� Tick https://dev.epicgames.com/community/learning/tutorials/qZYY/unreal-engine-on-tick-ability-task
}

bool UCPP_GA_Reload::CanReload()
{
	if (GetWeaponComponent()->IsMagazineFull() || GetWeaponComponent()->GetRemainRounds() == 0)
	{
		return false;
	}
	return true;
}

class UAnimMontage* UCPP_GA_Reload::GetCurrentHandsReloadMontage()
{
	if (GetWeaponComponent()->IsMagazineEmpty())
	{
		return GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("Reload"));
	}
	return GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("TacReload"));
}

class UAnimMontage* UCPP_GA_Reload::GetCurrentWeaponReloadMontage()
{
	if (GetWeaponComponent()->IsMagazineEmpty())
	{
		return GetWeaponComponent()->Get_FP_Weapon_Montage(TEXT("Reload"));
	}
	return GetWeaponComponent()->Get_FP_Weapon_Montage(TEXT("TacReload"));

}

float UCPP_GA_Reload::GetCurrentTPReloadRate()
{
	return GetWeaponComponent()->IsMagazineEmpty() ? 0.8f : 1.0f;
}

