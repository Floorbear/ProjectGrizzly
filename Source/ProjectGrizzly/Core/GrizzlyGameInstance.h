// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "..\Weapon/WeaponData.h"
#include "ProjectGrizzly/Item/CPP_Item.h"
#include "Widgets/Views/STreeView.h"
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
	UDataTable* GetWeaponDT() const
	{
		return WeaponDT;
	}

	UFUNCTION()
	UDataTable* GetWeaponAnimDT() const
	{
		return WeaponAnimDT;
	}

	UFUNCTION()
	UDataTable* GetItemDT() const
	{
		return ItemDT;
	}
private:
	UPROPERTY()
	UDataTable* WeaponDT = nullptr;
	UPROPERTY()
	UDataTable* WeaponAnimDT = nullptr;
	UPROPERTY()
	UDataTable* ItemDT = nullptr;

	//--------------------------------------------------------------------------------------------------
	//										DropTable
	//--------------------------------------------------------------------------------------------------
public:
	TMap<FName,TArray<FItemData*>> GetDropTable();


private:
	void InitDropTable();
	TMap<FName,TArray<FItemData*>> DropTable;
	
};
