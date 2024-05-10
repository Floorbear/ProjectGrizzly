// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GrizzlyPC.generated.h"

/**
 * 
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API AGrizzlyPC : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void OnDeadNotificationUI();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OffDeadNotificationUI();

	UFUNCTION(BlueprintCallable)
	bool IsMyComputer();
};
