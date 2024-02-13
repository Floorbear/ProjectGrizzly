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
private:
	UFUNCTION()
	void OnBlendOut();
	
protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

public:
	UCPP_GA_Reload();
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Reload")
	class UAnimMontage* ReloadMontage;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Reload")
	class UAnimMontage* TacReloadMontage;
	
};
