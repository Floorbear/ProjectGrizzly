// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_Sprint : public UGrizzlyAbility
{
	GENERATED_BODY()

public:
	UCPP_GA_Sprint();
	

	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	//앉은 상태에서 달리기할 때 검사
	bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> GE_Sprint;

	FActiveGameplayEffectHandle SprintHandle;

	UFUNCTION()
	void TimerTick();

	UFUNCTION()
	void OnCrouchCheck();

	// ------------------------------------------
	//					AI
	// ------------------------------------------
	UFUNCTION()
	void OnReceiveUnSprintEvent(FGameplayEventData Payload);
};
