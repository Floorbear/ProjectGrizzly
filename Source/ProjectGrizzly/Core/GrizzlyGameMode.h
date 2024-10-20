// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GrizzlyGameMode.generated.h"

/**
 *
 */
UCLASS()
class PROJECTGRIZZLY_API AGrizzlyGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

	virtual void PostLogin(APlayerController* NewPlayer);

	virtual void StartPlay();

};
