// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_SwapWeapon.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "ProjectGrizzly/Character/CPP_PlayableCharacter.h"
#include "ProjectGrizzly/Weapon/WeaponComponent.h"

UCPP_GA_SwapWeapon::UCPP_GA_SwapWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::SwapWeapon;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.SwapWeapon")));
	
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Reload")));
	
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Shoot")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Reload")));

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	
}

void UCPP_GA_SwapWeapon::DoUndraw()
{
	GetCharacter()->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(GetCurrentWeaponUndrawAM());
	auto* Task = UAbilityTask_WaitGameplayEvent::
		WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.SwapWeapon.OnUndrawCompleted")));
	Task->EventReceived.AddDynamic(this,&UCPP_GA_SwapWeapon::OnUndrawCompleted);
	Task->ReadyForActivation();
}

void UCPP_GA_SwapWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	if(!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(IsUnarmed(SourceWeaponInstance))
	{
		// Do Draw
		DoDraw();
		return;
	}
	
	// Do UnDraw
	DoUndraw();
}

bool UCPP_GA_SwapWeapon::CommitCheck(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	if(!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
		return false;
	
	TargetSlot= GetCharacter()->GetCurrentWeaponSlot();
	
	TargetWeaponInstance = GetCharacter()->GetWeaponInstanceFromSlot(TargetSlot);
	SourceWeaponInstance = GetCharacter()->GetCurrentWeaponInstance();

	if(TargetWeaponInstance == SourceWeaponInstance)
	{
		return false;
	}

	return  true;
}

void UCPP_GA_SwapWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	TargetWeaponInstance = nullptr;
	SourceWeaponInstance = nullptr;

}

bool UCPP_GA_SwapWeapon::IsUnarmed(class ACPP_WeaponInstance* _WeaponInstance) const
{
	return GetCharacter()->IsUnarmed(_WeaponInstance);
}

UAnimMontage* UCPP_GA_SwapWeapon::GetCurrentWeaponUndrawAM() const
{
	UAnimMontage* AM = GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("Undraw"));
	checkf(AM,TEXT("Cant find Am"));
	return  AM;
}

UAnimMontage* UCPP_GA_SwapWeapon::GetCurrentWeaponDrawAM() const
{
	UAnimMontage* AM = GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("Draw"));
	checkf(AM,TEXT("Cant find Am"));
	return  AM;
}

void UCPP_GA_SwapWeapon::DoDraw()
{
	GetCharacter()->SwapWeapon(TargetSlot);
	GetCharacter()->GetWeaponComponent()->SetWeapon_Inner(TargetWeaponInstance);
	if(IsUnarmed((TargetWeaponInstance)))
	{
		K2_EndAbility();
		return;
	}
	//어색하면 타이머 설정하기



	//바뀐 무기를 들어 올리기
	FAlphaBlendArgs BlendArgs;
	BlendArgs.BlendTime = 0;
	GetCharacter()->GetHandsMeshComponent()->GetAnimInstance()->Montage_PlayWithBlendIn(GetCurrentWeaponDrawAM(),BlendArgs);
	auto* EndTask = UAbilityTask_WaitGameplayEvent::
		WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.SwapWeapon.OnDrawCompleted")));
	EndTask->EventReceived.AddDynamic(this,&UCPP_GA_SwapWeapon::OnDrawCompleted);
	EndTask->ReadyForActivation();

	auto* StartTask = UAbilityTask_WaitGameplayEvent::
		WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.SwapWeapon.OnDrawStarted")));
	StartTask->EventReceived.AddDynamic(this,&UCPP_GA_SwapWeapon::OnDrawStarted);
	StartTask->ReadyForActivation();
	

	//렌더링 off
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
}

void UCPP_GA_SwapWeapon::OnUndrawCompleted(FGameplayEventData _Data)
{
	DoDraw();
}

void UCPP_GA_SwapWeapon::OnDrawCompleted(FGameplayEventData _Data)
{
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
	K2_EndAbility();
}

void UCPP_GA_SwapWeapon::OnDrawStarted(FGameplayEventData _Data)
{
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
}
