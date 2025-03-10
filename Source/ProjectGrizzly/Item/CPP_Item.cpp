// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Item.h"
#include "..\Core/GrizzlyGameInstance.h"
#include "ProjectGrizzly/Weapon/CPP_Ammo.h"
#include "..\Character/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGrizzly/Weapon/CPP_WeaponInstance.h"


ACPP_Item::ACPP_Item()
{
	static auto DT = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Item/DT_Item.DT_Item"));
	ItemDT = DT.Object;
	bReplicates = true;
	bAlwaysRelevant = true;
}

ACPP_Item* ACPP_Item::CreateItem(FName _ItemRowName,UObject* _WorldContext, int _Amount, UInventoryComponent* _Parent)
{
	checkf(_WorldContext != nullptr, TEXT("_WorldContext is null"));
	//빈 아이템에 대한 예외처리
	if(_ItemRowName.Compare(TEXT("Empty")) == 0)
	{
		ACPP_Item* EmptyItem = CreateItem_Inner<ACPP_Item>(_ItemRowName,_WorldContext,_Amount);
		EmptyItem->bReplicates = false;
		return EmptyItem;
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
		ACPP_WeaponInstance* NewWeapon = CreateItem_Inner<ACPP_WeaponInstance>(_ItemRowName,_WorldContext,_Amount,_Parent);
		return NewWeapon;
	}

	//탄약
	if(Prefix.Compare(TEXT("Ammo")) == 0)
	{
		ACPP_Ammo* NewAmmo = CreateItem_Inner<ACPP_Ammo>(_ItemRowName,_WorldContext,_Amount,_Parent);
		return NewAmmo;
	}
	auto* NewItem = CreateItem_Inner<ACPP_Item>(_ItemRowName,_WorldContext,_Amount,_Parent);
	return NewItem;
}

void ACPP_Item::Init(FName _RowName)
{
	FItemData* Data = GetItemDT()->FindRow<FItemData>(_RowName, FString(""));
	ensure(Data != NULL); // _ItemRowName 이 DT_Item의 이름에 등록돼있지 않음
	ItemData = *Data;
}

UDataTable* ACPP_Item::GetItemDT() const
{
	check(ItemDT != nullptr);
	return ItemDT;
}


void ACPP_Item::SetParent_Implementation(class UInventoryComponent* _Parent)
{
	Parent = _Parent;
	check(Parent->GetOwner());
	SetOwner(Parent->GetOwner());
}

void ACPP_Item::CheckConditionAndRemove_Implementation()
{
	if(!IsEmpty() || Parent == nullptr)
	{
		return;
	}
	Parent->RemoveItemInstanceFromInventory(this);
}

bool ACPP_Item::IsUsable() const
{
	return GetItemData().Interactions.Contains(EItemInteraction::Use);
}

void ACPP_Item::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_Item, Amount);
	DOREPLIFETIME(ACPP_Item, ItemData);
	DOREPLIFETIME(ACPP_Item, Parent);
}

bool ACPP_Item::HasNetOwner() const
{
	if(!Parent)
		return false;

	return Parent->HasNetOwner();
}




