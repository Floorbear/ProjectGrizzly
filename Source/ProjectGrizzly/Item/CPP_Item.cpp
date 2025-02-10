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
	//�� �����ۿ� ���� ����ó��
	if(_ItemRowName.Compare(TEXT("Empty")) == 0)
	{
		return CreateItem_Inner<UCPP_Item>(_ItemRowName,_Amount);
	}
	
	//���λ� _  �Ľ�
	FString ItemRowName = _ItemRowName.ToString();
	FString Prefix;
	FString Suffix;
	// _ �� �������� ������ ����
	bool bSuccess = ItemRowName.Split(TEXT("_"),&Prefix,&Suffix);
	ensureMsgf(bSuccess,TEXT("%s do not contain _"),*ItemRowName);

	//����
	if(Prefix.Compare(TEXT("Weapon")) == 0)
	{
		UCPP_WeaponInstance* NewWeapon = CreateItem_Inner<UCPP_WeaponInstance>(_ItemRowName,_Amount);
		return NewWeapon;
	}

	//ź��
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
	ensure(Data != NULL); // _ItemRowName �� DT_Item�� �̸��� ��ϵ����� ����
	ItemData = *Data;
}

UDataTable* UCPP_Item::GetItemDT() const
{
	check(ItemDT != nullptr);
	return ItemDT;
}

