// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception\AIPerceptionComponent.h"
#include "CPP_PGCharacter.h"
#include "..\Environment\CPP_PatrolPoint.h"
#include "CPP_AI_PlayableCharacter_PC.generated.h"


/**
 *
 */

class UPlayerInventoryComponent;

UENUM(BlueprintType)
enum class EDifficulty : uint8
{
	Novice				UMETA(DisplayName = "Novice"),
	Standard			UMETA(DisplayName = "Standard"),
	Expert				UMETA(DisplayName = "Expert"),
	Laser				UMETA(DisplayName = "Laser")
};

static EDifficulty GetDifficulty()
{
	return EDifficulty::Expert;
}

USTRUCT(BlueprintType)
struct  FThreatenResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FThreatenResult")
	float ThreatValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FThreatenResult")
	AController* Investigator = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FThreatenResult")
	class ACPP_PGCharacter* Causer = NULL;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_OnThreatened, FThreatenResult&, ThreatenResult);


UCLASS()
class PROJECTGRIZZLY_API ACPP_AI_PlayableCharacter_PC : public AAIController
{
	GENERATED_BODY()
public:
	ACPP_AI_PlayableCharacter_PC();


	void PostInitializeComponents() override;

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true);

	UFUNCTION(BlueprintCallable)
	ACharacter* GetTargetActor();

protected:
	void Tick(float DeltaTime) override;
	void BeginPlay() override;
	UFUNCTION()
	void UpdateTargetLocation();
	// ----------------------------------
	//				Perception
	// ----------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool TraceToTargetLocation() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
	float MaxLostTargetActorTime = 10.f;

	UFUNCTION(BlueprintCallable, Category = "Perception")
	void UpdateSense();


protected:
	//타겟에게 Focus했을때 Focus하는 위치
	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;
private:
	void TransmitTargetInformation(AActor* _Actor, FAIStimulus _Stimulus);
	void ReceiveTargetInformation(AActor* _Actor, FAIStimulus _Stimulus);
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* _Actor, FAIStimulus _Stimulus);

	UFUNCTION(BlueprintCallable)
	void UpdateSightPerception(AActor* _Actor, FAIStimulus _Stimulus);

	UFUNCTION(BlueprintCallable)
	ACPP_PGCharacter* CalculateTargetActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	float LostTargetActorTime = 10.f;

	FTimerHandle LostTargetActor_TimerHandle;

	FRotator SmoothTargetRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	float SmoothTargetRotationSpeed = 180.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	TMap<ACPP_PGCharacter*, FAIStimulus> PerceptionTable;

	// ----------------------------------
	//				Threat
	// ----------------------------------
public:
	UPROPERTY(EditAnywhere, BlueprintCallable, Category = "Threat")
	FDele_OnThreatened FOnThreatened;
	UFUNCTION(BlueprintCallable)
	float GetBulletThreat();
	UFUNCTION(BlueprintCallable)
	void ApplyThreat(UPARAM(ref) FThreatenResult& _ThreatenResult);
	UFUNCTION(BlueprintCallable)
	void SetBulletThreat(float _Value);

private:
	UFUNCTION()
	void OnThreatened(FThreatenResult& _ThreatenResult);

	//--------------------------------------------------------------------------------------------------
	//										Patrol
	//--------------------------------------------------------------------------------------------------
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrentPatrolIndex = 0;
	UFUNCTION(BlueprintCallable)
	TArray<ACPP_PatrolPoint* > GetPatrolRoute() const
	{
		return PatrolRoute;
	}

	UFUNCTION(BlueprintCallable,Category = Patrol)
	void SetPatrolLocation(FVector& _Location);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName RouteName = FName(TEXT("Default"));

	void UpdatePatrol();
private:
	


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<ACPP_PatrolPoint*> PatrolRoute;

	UDataTable* PatrolDT;
	// ----------------------------------
	//				Difficulty
	// ----------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty", meta = (AllowPrivateAccess = "true"))
	EDifficulty Difficulty;
public:
	EDifficulty GetDifficulty() const
	{
		return Difficulty;
	}
public:
	//UPROPERTY(EditAnywhere,Category = Difficulty)
	//static TEnumAsByte<enum EDifficulty> CurrentDifficulty; //ToDo : GameInstance로 가야함

	//--------------------------------------------------------------------------------------------------
	//										Inventory
	//--------------------------------------------------------------------------------------------------
	

};
