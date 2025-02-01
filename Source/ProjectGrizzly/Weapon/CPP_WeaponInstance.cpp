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


