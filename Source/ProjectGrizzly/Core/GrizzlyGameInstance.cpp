// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyGameInstance.h"

void UGrizzlyGameInstance::Init()
{
	Super::Init();

}

UGrizzlyGameInstance::UGrizzlyGameInstance()
{
	//WeaponDataTable & WeaponAnimTable ·Îµù
	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponDataTable.WeaponDataTable"));
		check(IsValid(DataTable.Object));
		WeaponDataTable = DataTable.Object;
	}
	{
		static auto DataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponAnimTable.WeaponAnimTable"));
		check(IsValid(DataTable.Object));
		WeaponAnimTable = DataTable.Object;
	}

}

