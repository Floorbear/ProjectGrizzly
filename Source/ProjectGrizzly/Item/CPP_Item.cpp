// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Item.h"
#include "..\Core/GrizzlyGameInstance.h"
#include "ProjectGrizzly/Weapon/CPP_Ammo.h"
#include "ProjectGrizzly/Weapon/CPP_WeaponInstance.h"


UCPP_Item::UCPP_Item()
{
	static auto DT = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Item/DT_Item.DT_Item"));
	ItemDT = DT.Object;
}

UCPP_Item* UCPP_Item::CreateItem(FName _ItemRowName, int _Amount)
{
	//빈 아이템에 대한 예외처리
	if(_ItemRowName.Compare(TEXT("Empty")) == 0)
	{
		return CreateItem_Inner<UCPP_Item>(_ItemRowName,_Amount);
	}
	
	//접두사 _  파싱
	FString ItemRowName = _ItemRowName.ToString();
	FString Prefix;
	FString Suffix;
	// _ 가 존재하지 않으면 오류
	bool bSuccess = ItemRowName.Split(TEXT("_"),&Prefix,&Suffix);
	ensureMsgf(bSuccess,TEXT("%s do not contain _"),*ItemRowName);

	//무기
	if(Prefix.Compare(TEXT("Weapon")) == 0)
	{
		UCPP_WeaponInstance* NewWeapon = CreateItem_Inner<UCPP_WeaponInstance>(_ItemRowName,_Amount);
		return NewWeapon;
	}

	//탄약
	if(Prefix.Compare(TEXT("Ammo")) == 0)
	{
		UCPP_Ammo* NewAmmo = CreateItem_Inner<UCPP_Ammo>(_ItemRowName,_Amount);
		return NewAmmo;
	}
	return CreateItem_Inner<UCPP_Item>(_ItemRowName,_Amount);
}

void UCPP_Item::Init(FName _RowName)
{
	FItemData* Data = GetItemDT()->FindRow<FItemData>(_RowName, FString(""));
	ensure(Data != NULL); // _ItemRowName 이 DT_Item의 이름에 등록돼있지 않음
	ItemData = *Data;
}

UDataTable* UCPP_Item::GetItemDT() const
{
	check(ItemDT != nullptr);
	return ItemDT;
}

