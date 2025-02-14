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
class PROJECTGRIZZLY_API ACPP_WeaponInstance : public ACPP_Item
{
	GENERATED_BODY()
public:
	ACPP_WeaponInstance();
	void InitWeapon();


	//--------------------------------------------------------------------------------------------------
	//										Item
	//--------------------------------------------------------------------------------------------------
	void Init(FName _RowName) override;
	
protected:
	bool CanRender() const override;

private:
	UDataTable* WeaponDataDT = nullptr;
public:
	FWeaponData* GetWeaponData() const;

	UFUNCTION(BlueprintCallable)
	FWeaponData K2_GetWeaponData() const;

	//--------------------------------------------------------------------------------------------------
	//										Ammo
	//--------------------------------------------------------------------------------------------------
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

	FName GetAmmoName() const;
private:
	
	FWeaponData* WeaponData = nullptr;
	UPROPERTY(Replicated)
	int Rounds = 0;



	//--------------------------------------------------------------------------------------------------
	//										Equip
	//--------------------------------------------------------------------------------------------------
	// Equip doesnt mean Current drawing Weapon. It means Equipped Primary Weapon & Secondary Weapon
public:
	UFUNCTION(BlueprintCallable)
	bool IsEquipped() const
	{
		return bIsEquipped;
	}

	UFUNCTION(BlueprintCallable)
	void SetEquipped(const bool IsEquipped)
	{
		bIsEquipped = IsEquipped;
	}
private:
	UPROPERTY(ReplicatedUsing = OnRep_IsEquipped)
	bool bIsEquipped;

	//--------------------------------------------------------------------------------------------------
	//										FireMode
	//--------------------------------------------------------------------------------------------------
private:
	TArray<EWeaponMode> WeaponModeSet;
	int CurrentWeaponModeIndex = 0;
public:
	EWeaponMode GetCurrentWeaponMode() const
	{
		return WeaponModeSet[CurrentWeaponModeIndex];
	}
	
	//--------------------------------------------------------------------------------------------------
	//										Network
	//--------------------------------------------------------------------------------------------------
	virtual void OnRep_ItemData() override;
	UFUNCTION()
	void OnRep_IsEquipped() const;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
