// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Shoot.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_Shoot : public UGrizzlyAbility
{
	GENERATED_BODY()

public:
	UCPP_GA_Shoot();
	
protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
