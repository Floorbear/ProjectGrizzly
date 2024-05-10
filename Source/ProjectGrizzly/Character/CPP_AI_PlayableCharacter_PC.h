// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception\AIPerceptionComponent.h"
#include "CPP_AI_PlayableCharacter_PC.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API ACPP_AI_PlayableCharacter_PC : public AAIController
{
	GENERATED_BODY()
public:
		ACPP_AI_PlayableCharacter_PC();


	void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	ACharacter* GetTargetActor();

	UFUNCTION()
	void UpdateTargetLocation();
	// ----------------------------------
	//				Perception
	// ----------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
	float MaxLostTargetActorTime = 10.f;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerception;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* _Actor, FAIStimulus _Stimulus);

	UFUNCTION(BlueprintCallable)
	void UpdateSightPerception(AActor* _Actor, FAIStimulus _Stimulus);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	float LostTargetActorTime = 10.f;

	FTimerHandle LostTargetActor_TimerHandle;

	// ----------------------------------
	//				Threat
	// ----------------------------------
public:
	UFUNCTION(BlueprintCallable)
	float GetBulletThreat();
	UFUNCTION(BlueprintCallable)
	void SetBulletThreat(float _Value);
};
