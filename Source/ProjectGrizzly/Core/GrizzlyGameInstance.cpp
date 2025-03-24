// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyGameInstance.h"
#include "..\Character/PlayerInventoryComponent.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
#include "ProjectGrizzly/Item/CPP_Item.h"

void UGrizzlyGameInstance::Init()
{
	Super::Init();
	// FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UGrizzlyGameInstance::OnWorldInitialized);
	
	FInventoryData PlayerInventory;
	PlayerInventory.InventoryName = TEXT("Player");

	FInventoryData StashInventory;
	StashInventory.InventoryName = TEXT("Stash");

	TArray<FInventoryEntry> ItemSet;
	FInventoryEntry Colt;
	Colt.ItemName = TEXT("Weapon_Colt");
	Colt.Amount = 1;
	ItemSet.Add(Colt);
	FInventoryEntry AKM;
	AKM.ItemName = TEXT("Weapon_AKM");
	AKM.Amount = 1;
	ItemSet.Add(AKM);

	FInventoryEntry ColtAmmo;
	ColtAmmo.ItemName = TEXT("Ammo_45ACP");
	ColtAmmo.Amount = 100;
	ItemSet.Add(ColtAmmo);

	FInventoryEntry AKMAmmo;
	AKMAmmo.ItemName = TEXT("Ammo_7.62x39mm");
	AKMAmmo.Amount = 300;
	ItemSet.Add(AKMAmmo);

	FInventoryEntry Medickit;
	Medickit.ItemName = TEXT("Medical_Medickit");
	Medickit.Amount = 10;
	ItemSet.Add(Medickit);

	StashInventory.InventoryData = ItemSet;
	
	SetInventory(PlayerInventory);
	SetInventory(StashInventory);
}

UGrizzlyGameInstance::UGrizzlyGameInstance()
{
	//WeaponDataTable & WeaponAnimTable 로딩
	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponDataTable.WeaponDataTable"));
		check(IsValid(DataTable.Object));
		WeaponDT = DataTable.Object;
	}
	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponAnimTable.WeaponAnimTable"));
		check(IsValid(DataTable.Object));
		WeaponAnimDT = DataTable.Object;
	}

	//Item DT 로딩
	{
		static auto DT = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Item/DT_Item.DT_Item"));
		ItemDT = DT.Object;
	}
	Grizzly_LOG(TEXT("GrizzlyInstance is loaded"));

}

void UGrizzlyGameInstance::OnWorldInitialized(UWorld* _World, const UWorld::InitializationValues _InitializationValues)
{
	if(IsDedicatedServerInstance() && !bIsTraveled)
	{
		GetWorld()->ServerTravel(TEXT("/Game/FirstPerson/Maps/FirstPersonMap.FirstPersonMap?listen"));
	}
}

TMap<FName, TArray<FItemData*>> UGrizzlyGameInstance::GetDropTable()
{
	if(DropTable.IsEmpty())
	{
		InitDropTable();
	}
	return DropTable;
}

void UGrizzlyGameInstance::InitDropTable()
{
	//DropTable이 이미 초기화됐는지 확인
	if(!DropTable.IsEmpty())
		return;
	
	TArray<FItemData*> AllRowsItemData;
	ItemDT->GetAllRows<FItemData>(TEXT("__FUNCTION__"),AllRowsItemData);

	for(FItemData* ItemData : AllRowsItemData)
	{
		if(ItemData->ItemRarity == EItemRarity::NoDrop)
			continue;
		
		if(!DropTable.Contains(ItemData->Type))
		{
			DropTable.Add(ItemData->Type,{ItemData});
		}
		else
		{
			DropTable[ItemData->Type].Add(ItemData);
		}
	}
}

const FInventoryData& UGrizzlyGameInstance::GetInventoryFromKey(FName _Key)
{
	testcount++;
	int a = testcount;
	FInventoryData* InventoryData = FindInventoryByKey(_Key);
	if(!InventoryData)
	{
		Grizzly_LOG(TEXT("%s is not included in Instance's Inventory"),*_Key.ToString());
	}

	return *InventoryData;
}

void UGrizzlyGameInstance::SetInventory(FInventoryData& _InventoryData)
{
	FInventoryData* InventoryData = FindInventoryByKey(_InventoryData.InventoryName);
	if(InventoryData)
	{
		*InventoryData = _InventoryData;
	}
	else
	{
		Inventory.Add(_InventoryData);
	}
}

FInventoryData* UGrizzlyGameInstance::FindInventoryByKey(FName _Key)
{
	FInventoryData* InventoryData = nullptr;
	for(int i = 0; i < Inventory.Num(); i++)
	{
		if(Inventory[i].InventoryName == _Key)
		{
			InventoryData = &Inventory[i];
			break;
		}
	}

	return InventoryData;
}



