// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_WeaponInstance.h"
#include "..\Core/GrizzlyGameInstance.h"


UCPP_WeaponInstance::UCPP_WeaponInstance()
{
}

UCPP_WeaponInstance* UCPP_WeaponInstance::CreateWeaponInstance(FName _WeaponName, UWorld* _World)
{
	UCPP_WeaponInstance* NewWeapon = NewObject<UCPP_WeaponInstance>();
	UGrizzlyGameInstance* GameInstance = Cast<UGrizzlyGameInstance>(_World->GetGameInstance());
	NewWeapon->WeaponData = GameInstance->GetWeaponDataTable()->FindRow<FWeaponData>(_WeaponName, FString(""));
	
	ensure(NewWeapon->WeaponData != NULL); // WeaponName 에 해당하는 데이터가 존재하지 않음
	return NewWeapon;
}


