// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_WeaponInstance.h"
#include "..\Core/GrizzlyGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGrizzly/Character/InventoryComponent.h"


ACPP_WeaponInstance::ACPP_WeaponInstance()
{
	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponDataTable.WeaponDataTable"));
		check(IsValid(DataTable.Object));
		WeaponDataDT = DataTable.Object;
	}
	bReplicates = true;
}

void ACPP_WeaponInstance::InitWeapon()
{
	FName WeaponName = GetItemData().NameWithOutPrefix;
	WeaponData = WeaponDataDT->FindRow<FWeaponData>(WeaponName, FString(""));
	checkf(WeaponData != nullptr, TEXT("%s is not included in DT"),WeaponName);
	Rounds = WeaponData->Rounds;
	WeaponModeSet = WeaponData->WeaponModes;
	InstanceMode = true;
}

void ACPP_WeaponInstance::Init(FName _RowName)
{
	Super::Init(_RowName);
	InitWeapon();
}

bool ACPP_WeaponInstance::CanRender() const
{
	return !IsEquipped();
}

FWeaponData* ACPP_WeaponInstance::GetWeaponData() const
{
	ensure(WeaponData != nullptr);
	return WeaponData;
}

FWeaponData ACPP_WeaponInstance::K2_GetWeaponData() const
{
	ensure(WeaponData != nullptr);
	return *WeaponData;
}

FName ACPP_WeaponInstance::GetAmmoName() const
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE,TEXT("ECaliber"),true);
	//FText TAmmoName = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(GetWeaponData()->Caliber));
	FName TAmmoName = EnumPtr->GetNameByValue(static_cast<int64>(GetWeaponData()->Caliber));
	FString AmmoName = TAmmoName.ToString();
	//Split
	AmmoName.Split(TEXT("::"),nullptr,&AmmoName);

	//이상한 이름이 있는지 체크
	for(int i =0; i < EnumPtr->NumEnums(); i++)
	{
		//bool Equal = AmmoName.Equals(EnumPtr->GetDisplayNameTextByIndex(i).ToString());
		FString CurAmmoName = EnumPtr->GetNameByIndex(i).ToString();
		CurAmmoName.Split(TEXT("::"),nullptr,&CurAmmoName);
		bool Equal = AmmoName.Equals(CurAmmoName);
		if(Equal)
		{
			//접두사 추가
			AmmoName = TEXT("Ammo_") + AmmoName;
			return FName(AmmoName);
		}
	}
	checkf(false,TEXT("%s is not included in CaliberDisplayName"),*AmmoName);
	return FName(TEXT("Error"));

}

void ACPP_WeaponInstance::OnRep_ItemData()
{
	Super::OnRep_ItemData();
	InitWeapon();
}

void ACPP_WeaponInstance::OnRep_IsEquipped() const
{
	if(Parent == nullptr)
		return;
	Parent->OnInventoryChanged.Broadcast();
}

void ACPP_WeaponInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_WeaponInstance,EquippedSlot);
	DOREPLIFETIME(ACPP_WeaponInstance,Rounds);
}


