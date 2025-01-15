// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "CPP_WeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_WeaponInstance : public UObject
{
	GENERATED_BODY()
public:
	UCPP_WeaponInstance();

public:
	static UCPP_WeaponInstance* CreateWeaponInstance(FName _WeaponName, UWorld* _World);

public:
	FWeaponData* GetWeaponData() const
	{
		ensure(WeaponData != nullptr);
		return WeaponData;
	}

	UFUNCTION(BlueprintCallable)
	void SetRounds(int _NewRounds)
	{
		Rounds = FMath::Clamp(_NewRounds,0,999);
	}

	UFUNCTION(BlueprintCallable)
	int GetRounds() const
	{
		return Rounds;
	}
private:
	FWeaponData* WeaponData = nullptr;
	int Rounds;
};
