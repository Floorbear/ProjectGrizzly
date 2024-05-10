// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Crouch.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_Crouch : public UGrizzlyAbility
{
	GENERATED_BODY()

	public:
		UCPP_GA_Crouch();
		bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;


		void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnPressed(float _TimeWaited);
	UFUNCTION()
	void OnFinish();
	UFUNCTION()
	void OnDelayFinishWhenActivateSprint();

	public:
	void RequestUnCrouch();
	void RequestUnCrouchWhenActivateSprint();

	// ----------------------------------
	//				AI
	// ----------------------------------
	UFUNCTION()
	void OnReceiveUnCrouchEvent(FGameplayEventData Payload);

};
