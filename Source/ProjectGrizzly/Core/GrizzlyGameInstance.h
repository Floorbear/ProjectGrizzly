// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "..\Weapon/WeaponData.h"
#include "ProjectGrizzly/Item/CPP_Item.h"
#include "Widgets/Views/STreeView.h"
#include "..\Character/InventoryComponent.h"
#include "GrizzlyGameInstance.generated.h"



UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Loadout,
	Playing
};

class UPlayerInventoryComponent;
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
//										World
//--------------------------------------------------------------------------------------------------
	bool bIsTraveled = false;

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

	//--------------------------------------------------------------------------------------------------
	//										Inventory
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	const FInventoryData& GetInventoryFromKey(FName _Key);

	

	UFUNCTION(BlueprintCallable)
	void SetInventory(UPARAM(ref) FInventoryData& _InventoryData) ;

private:
	FInventoryData* FindInventoryByKey(FName _Key);

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = Inventory, meta = (AllowPrivateAccess = true))
	TArray<FInventoryData> Inventory;

	//--------------------------------------------------------------------------------------------------
	//										GamePhase
	//--------------------------------------------------------------------------------------------------
private:
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = GamePhase, meta = (AllowPrivateAccess = true))
	EGamePhase GamePhase;

public:
	UFUNCTION(BlueprintCallable, Category = GamePhase)
	EGamePhase GetGamePhase() const
	{
		return GamePhase;
	}
	
	UFUNCTION(BlueprintCallable, Category = GamePhase)
	void SetGamePhase(EGamePhase _GamePhase)
	{
		GamePhase = _GamePhase;
	}
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = test, meta = (AllowPrivateAccess = true))
	int testcount = 0;

	//--------------------------------------------------------------------------------------------------
	//										Faction & Model
	//--------------------------------------------------------------------------------------------------
private:
	EFaction MyFaction;
	ECharacterModel MyCharacterModel;
public:
	UFUNCTION(BlueprintCallable, Category = Faction)
	void SetFaction(EFaction _Faction)
	{
		MyFaction = _Faction;
	}

	UFUNCTION(BlueprintCallable, Category = Faction)
	EFaction GetFaction() const
	{
		return MyFaction;
	}

	UFUNCTION(BlueprintCallable,Category = Model)
	void SetCharacterModel(ECharacterModel _CharacterModel)
	{
		MyCharacterModel = _CharacterModel;
	}

	UFUNCTION(BlueprintCallable, Category = Model)
	ECharacterModel GetCharacterModel() const
	{
		return MyCharacterModel;
	}
};
