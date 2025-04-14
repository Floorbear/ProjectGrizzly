// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_PlayableCharacter.h"
#include "CPP_AIPlayableCharacter.generated.h"
/**
 *
 */

USTRUCT(BlueprintType)
struct FAIPlayableCharacterSpawnParameter
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EFaction Faction;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ECharacterModel Model;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Weapon;
};

UCLASS()
class PROJECTGRIZZLY_API ACPP_AIPlayableCharacter : public ACPP_PlayableCharacter
{
	GENERATED_BODY()

public:
	ACPP_AIPlayableCharacter();

	void Tick(float DeltaTime) override;

	bool IsDead() const override;

protected:
	void BeginPlay() override;

private:
	void HealthChanged(const struct FOnAttributeChangeData& Data);
	void SpeedChanged(const struct FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;
	//--------------------------------------------------------------------------------------------------
	//										Override
	//--------------------------------------------------------------------------------------------------
	virtual void PossessedBy(AController* NewController) override;

	//--------------------------------------------------------------------------------------------------
	//										SpawnAIActor
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable, Category="AI", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static ACPP_AIPlayableCharacter* SpawnAIPlayableCharacter(UObject* WorldContextObject,TSubclassOf<ACPP_AIPlayableCharacter> _Class, FAIPlayableCharacterSpawnParameter Parameter,UBehaviorTree* BehaviorTree, FVector Location, FRotator Rotation = FRotator::ZeroRotator, bool bNoCollisionFail = false, AActor* _Owner = nullptr);


	UFUNCTION(BlueprintCallable)
	float GetHealth() const;
	// ------------------------------------------------
	//					Interface
	// ------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	class ACPP_AI_PlayableCharacter_PC* GetAIController() const;
	//---------------------------------------------------
	//						Faction
	//---------------------------------------------------
public:
	EFaction GetFaction() const override;
	UFactionComponent* GetFactionComponent() const override
	{
		return FactionComponent;
	}
private:
	UPROPERTY(VisibleAnywhere)
	UFactionComponent* FactionComponent;
	// ----------------------------------
	//			Damage
	// ----------------------------------
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//--------------------------------------------------------------------------------------------------
	//										Alert
	//--------------------------------------------------------------------------------------------------
private:
	UPROPERTY(Replicated,BlueprintReadWrite,Category = Alert,meta = (AllowPrivateAccess = true))
	bool bIsAlert = true;
public:
	UFUNCTION(BlueprintCallable)
	bool IsAlert() const
	{
		return bIsAlert;
	}

	UFUNCTION()
	void CheckAlert();

	//--------------------------------------------------------------------------------------------------
	//										Network
	//--------------------------------------------------------------------------------------------------
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
