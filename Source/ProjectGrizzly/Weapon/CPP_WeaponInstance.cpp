// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_WeaponInstance.h"
#include "..\Core/GrizzlyGameInstance.h"


UCPP_WeaponInstance::UCPP_WeaponInstance()
{
	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponDataTable.WeaponDataTable"));
		check(IsValid(DataTable.Object));
		WeaponDataDT = DataTable.Object;
	}
}

void UCPP_WeaponInstance::Init(FName _RowName)
{
	Super::Init(_RowName);
	FName WeaponName = GetItemData().NameWithOutPrefix;
	WeaponData = WeaponDataDT->FindRow<FWeaponData>(WeaponName, FString(""));
	checkf(WeaponData != nullptr, TEXT("%s is not included in DT"),WeaponName);
	Rounds = WeaponData->Rounds;
	WeaponMode = WeaponData->WeaponModes[0];
	InstanceMode = true;
}

FWeaponData* UCPP_WeaponInstance::GetWeaponData() const
{
	ensure(WeaponData != nullptr);
	return WeaponData;
}

FWeaponData UCPP_WeaponInstance::K2_GetWeaponData() const
{
	ensure(WeaponData != nullptr);
	return *WeaponData;
}

FName UCPP_WeaponInstance::GetAmmoName() const
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE,TEXT("ECaliber"),true);
	FText TAmmoName = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(GetWeaponData()->Caliber));
	FString AmmoName = TAmmoName.ToString();

	//이상한 이름이 있는지 체크
	for(int i =0; i < EnumPtr->NumEnums(); i++)
	{
		bool Equal = AmmoName.Equals(EnumPtr->GetDisplayNameTextByIndex(i).ToString());
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


