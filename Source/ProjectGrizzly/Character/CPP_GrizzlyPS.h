// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "FactionComponent.h"
#include "CPP_GrizzlyPS.generated.h"

/**
 *
 */
UCLASS()
class PROJECTGRIZZLY_API ACPP_GrizzlyPS : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACPP_GrizzlyPS();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UGrizzlyAttributeSet* GetAttributeSet() const;

	// ----------------------------
	// ---------  Stat ------------
	// ----------------------------

	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetStamina() const;
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	int GetCurrentMagazineRounds() const;
	UFUNCTION(BlueprintCallable, Category = "Status")
	int GetRemainRounds() const;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ----------------------------
	// ----------G A S-------------
	// ----------------------------
public:
	bool bIsAbilityGiven = false;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemCompoent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	class UGrizzlyAttributeSet* AttributeSet;


	void BeginPlay() override;

	void HealthChanged(const struct FOnAttributeChangeData& Data);
	void SpeedChanged(const struct FOnAttributeChangeData& Data);

	// ------------------
	// ------ Dead ------
	// ------------------
public:
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

private:
	UPROPERTY(Replicated)
	bool bIsDead = false;

	// ----------------------------
	//			Faction
	// ----------------------------
public:
	UFUNCTION(BlueprintCallable)
	EFaction GetFaction() const;
	UFUNCTION(BlueprintCallable)
	UFactionComponent* GetFactionComponent() const
	{
		return FactionComponent;
	}
private:
	UPROPERTY(VisibleAnywhere)
	UFactionComponent* FactionComponent;

};
