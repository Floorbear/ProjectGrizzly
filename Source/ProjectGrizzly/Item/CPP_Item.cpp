// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Item.h"



UCPP_Item::UCPP_Item()
{
	const auto DT = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Item/DT_Item.DT_Item"));
	 Table = DT.Object;
}

UCPP_Item* UCPP_Item::CreateItem(FName _ItemRowName, int _Amount)
{
	UCPP_Item* NewItem = NewObject<UCPP_Item>();
	NewItem->Init(_ItemRowName);
	NewItem->Amount = _Amount;
	return NewItem;
}

void UCPP_Item::Init(FName _RowName)
{
	FItemData* Data = Table->FindRow<FItemData>(_RowName, FString(""));
	ensure(Data != NULL); // _ItemRowName 이 DT_Item의 이름에 등록돼있지 않음

	ItemData = *Data;
}

