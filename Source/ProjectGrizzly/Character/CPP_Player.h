// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_PlayableCharacter.h"
#include "CPP_Player.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_Player : public ACPP_PlayableCharacter
{
	GENERATED_BODY()

protected:
	void OnRep_PlayerState() override;
	

	void PossessedBy(AController* NewController) override;

	void BindASCInput();

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void BeginPlay() override;

};
