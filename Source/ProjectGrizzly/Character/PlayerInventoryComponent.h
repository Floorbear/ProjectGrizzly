// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "..\Weapon/CPP_WeaponInstance.h"
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
	// bIsPrimary : ÁÖ¹«Àå
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(UCPP_WeaponInstance* _WeaponInstance, bool bIsPrimary = true);

	UFUNCTION(BlueprintCallable)
	void UnEquipWeapon(bool bIsPrimary);
	
private:
	UPROPERTY()
	UCPP_WeaponInstance* PrimaryWeaponInstance = nullptr;
	UPROPERTY()
	UCPP_WeaponInstance* SecondaryWeaponInstance = nullptr;
	UPROPERTY()
	UCPP_WeaponInstance* UnarmedWeaponInstance = nullptr;
	
};
