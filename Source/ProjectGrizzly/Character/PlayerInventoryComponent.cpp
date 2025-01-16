// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"

#include "ProjectGrizzly/ProjectGrizzly.h"

void UPlayerInventoryComponent::EquipWeapon(UCPP_WeaponInstance* _WeaponInstance, bool bIsPrimary)
{
	if(bIsPrimary)
	{
		PrimaryWeaponInstance = _WeaponInstance;
		Grizzly_LOG(TEXT("%s is Equipped"),*_WeaponInstance->GetWeaponData()->WeaponName)
	}
	else
		SecondaryWeaponInstance = _WeaponInstance;
}


