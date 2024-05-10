// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PGDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UPGDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UPGDamageCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
