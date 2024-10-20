// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CPP_GrizzlyGameState.generated.h"

/**
 *
 */
UCLASS()
class PROJECTGRIZZLY_API ACPP_GrizzlyGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void HandleBeginPlay();
	virtual void OnRep_ReplicatedHasBegunPlay();
};
