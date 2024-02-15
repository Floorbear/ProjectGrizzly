// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TickTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);
/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UTickTask : public UAbilityTask
{
	GENERATED_BODY()
	


public:
	UTickTask();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UTickTask* CreateTickTask(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName
		);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;

private:
	float TaskDeltaTime;
};
