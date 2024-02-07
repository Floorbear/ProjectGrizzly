// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_PGCharacter.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_PGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACPP_PGCharacter();




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ------ Network -----
	UFUNCTION(BlueprintCallable)
	ENetRole GetNetRole();

	UFUNCTION(BlueprintCallable)
	bool IsMyComputer(); // �̱�, ��Ƽ �� �� ���
};
