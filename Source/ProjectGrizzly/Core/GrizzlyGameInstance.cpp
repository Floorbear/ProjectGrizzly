// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyGameInstance.h"

#include "ProjectGrizzly/ProjectGrizzly.h"
#include "ProjectGrizzly/Item/CPP_Item.h"

void UGrizzlyGameInstance::Init()
{
	Super::Init();

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

