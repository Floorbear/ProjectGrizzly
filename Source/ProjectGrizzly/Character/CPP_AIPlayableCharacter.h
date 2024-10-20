// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_PlayableCharacter.h"
#include "CPP_AIPlayableCharacter.generated.h"
/**
 *
 */
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
	

};
