// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "..\Weapon/CPP_WeaponInstance.h"
#include "..\Weapon/CPP_Ammo.h"
#include "PlayerInventoryComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UPlayerInventoryComponent : public UInventoryComponent
{
	GENERATED_BODY()

	void BeginPlay() override;

	//--------------------------------------------------------------------------------------------------
	//										Weapon
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	void DrawWeapon(bool _IsPrimary);
	// bIsPrimary : 주무장
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(UCPP_WeaponInstance* _WeaponInstance, bool bIsPrimary = true);

	UFUNCTION(BlueprintCallable)
	void UnEquipWeapon(UCPP_WeaponInstance* WeaponInstance);
	
	void UnEquipWeapon(bool bIsPrimary);

	void InitWeaponInstanceToUnarmedInstance();
	
private:
	//
	UCPP_Ammo* FindMatchingAmmo(const UCPP_WeaponInstance* _WeaponInstance);
	//비무장 상태면 Unarmed Instance로 할당됨
	UPROPERTY()
	UCPP_WeaponInstance* PrimaryWeaponInstance = nullptr;
	UPROPERTY()
	UCPP_WeaponInstance* SecondaryWeaponInstance = nullptr;
	//--------------------------------------------------------------------------------------------------
	//										Interface
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	static bool IsUnarmedInstance(const UCPP_WeaponInstance* _WeaponInstance);

	UFUNCTION(BlueprintCallable)
	UCPP_WeaponInstance* GetUnarmedInstance() const;

	// 0 : Primary , 1 : Secondary
	UFUNCTION(BlueprintCallable)
	TArray<UCPP_WeaponInstance*> GetWeaponInstances() const
	{
		return {PrimaryWeaponInstance,SecondaryWeaponInstance};
	}
	
};
