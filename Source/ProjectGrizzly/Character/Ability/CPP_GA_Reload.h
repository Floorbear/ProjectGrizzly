// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Reload.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_Reload : public UGrizzlyAbility
{
	GENERATED_BODY()
public:
	UCPP_GA_Reload();

	bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;


	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

private:
	UFUNCTION()
	void OnCompleted();

	bool bIsBindDelegate = false;
	UFUNCTION()
	void BindReloadMontage();
	
protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ActivateAbility();

	bool CanReload();
public:


	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetCurrentHandsReloadMontage();
	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetCurrentWeaponReloadMontage();
	UFUNCTION(BlueprintCallable)
	float GetCurrentTPReloadRate();

	
};
