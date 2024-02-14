// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CPP_A_PGCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadStartDelegate);
/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_A_PGCharacter : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable,BlueprintCallable,EditDefaultsOnly,Category = "Reload")
	FOnReloadStartDelegate OnReloadStartDelegate;
private:
	UFUNCTION()
	void AnimNotify_OnReloadStart();

};
