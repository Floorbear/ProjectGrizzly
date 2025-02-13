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
	//It means After Function EquipWeapon is called, Draw the equipped Weapon
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void DrawWeapon(bool _IsPrimary);
	
	// bIsPrimary : Main Weapon 
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void EquipWeapon(ACPP_WeaponInstance* _WeaponInstance, bool bIsPrimary = true);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void UnEquipWeapon(ACPP_WeaponInstance* WeaponInstance);
	
	void UnEquipWeapon(bool bIsPrimary);

public:
	UFUNCTION(Server,reliable)
	void InitWeaponInstanceToUnarmedInstance();
	
private:
	//
	ACPP_Ammo* FindMatchingAmmo(const ACPP_WeaponInstance* _WeaponInstance);
	//비무장 상태면 Unarmed Instance로 할당됨
	UPROPERTY(ReplicatedUsing = OnRep_WeaponInstance)
	ACPP_WeaponInstance* PrimaryWeaponInstance = nullptr;
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponInstance)
	ACPP_WeaponInstance* SecondaryWeaponInstance = nullptr;
	
	UPROPERTY(Replicated)
	ACPP_WeaponInstance* UnarmedInstance = nullptr;
	//--------------------------------------------------------------------------------------------------
	//										Interface
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool IsUnarmedInstance(const ACPP_WeaponInstance* _WeaponInstance) const;

	UFUNCTION(BlueprintCallable)
	ACPP_WeaponInstance* GetUnarmedInstance() const;

	// 0 : Primary , 1 : Secondary
	UFUNCTION(BlueprintCallable)
	TArray<ACPP_WeaponInstance*> GetWeaponInstances() const
	{
		return {PrimaryWeaponInstance,SecondaryWeaponInstance};
	}

	//--------------------------------------------------------------------------------------------------
	//										Network
	//--------------------------------------------------------------------------------------------------
private:
	UFUNCTION()
	void OnRep_WeaponInstance() const;
	UFUNCTION()
	void OnRep_OnInventoryChanged();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
};
