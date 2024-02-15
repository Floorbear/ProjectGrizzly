// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_ADS.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_ADS : public UGrizzlyAbility
{
	GENERATED_BODY()

public:
	UCPP_GA_ADS();

	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;


	void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "ADS",meta = (AllowPrivateAccess = "true"))
	bool bADS = true;

	bool bShouldCancel = false;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "ADS",meta = (AllowPrivateAccess = "true"))
	FVector IdleHandLocation = {3.8f,6.f,-1.8f};

	UFUNCTION()
	void Tick(float _DeltaTime);

	UFUNCTION(BlueprintCallable)
	float GetADSSpeed();

};
