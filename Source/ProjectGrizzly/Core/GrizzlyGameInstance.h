// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "..\Weapon/WeaponData.h"
#include "GrizzlyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UGrizzlyGameInstance : public UGameInstance
{
public:
	virtual void Init() override;

private:
	GENERATED_BODY()

public:
	UGrizzlyGameInstance();

//--------------------------------------------------------------------------------------------------
//										Weapon
//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION()
	UDataTable* GetWeaponDataTable() const
	{
		return WeaponDataTable;
	}

	UFUNCTION()
	UDataTable* GetWeaponAnimTable() const
	{
		return WeaponAnimTable;
	}

private:
	UPROPERTY()
	UDataTable* WeaponDataTable = nullptr;
	UPROPERTY()
	UDataTable* WeaponAnimTable = nullptr;
	
	
};
