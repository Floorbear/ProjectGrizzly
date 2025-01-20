// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "..\Item/CPP_Item.h"
#include "CPP_WeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_WeaponInstance : public UCPP_Item
{
	GENERATED_BODY()
public:
	UCPP_WeaponInstance();
	

	//--------------------------------------------------------------------------------------------------
	//										Item
	//--------------------------------------------------------------------------------------------------
	void Init(FName _RowName) override;
protected:

private:
	UDataTable* WeaponDataDT = nullptr;
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
	int Rounds = 0;

	EWeaponMode WeaponMode;
};
