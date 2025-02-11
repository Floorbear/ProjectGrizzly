// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"
#include "CPP_Player.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
#include "GrizzlyPC.h"
#include "ProjectGrizzly/Weapon/WeaponComponent.h"

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InitWeaponInstanceToUnarmedInstance();

}

void UPlayerInventoryComponent::DrawWeapon(bool _IsPrimary)
{
	UCPP_WeaponInstance* CurrentWeaponInstance = nullptr;
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	
	if(_IsPrimary)
	{
		CurrentWeaponInstance = PrimaryWeaponInstance;
	}
	else
	{
		CurrentWeaponInstance = SecondaryWeaponInstance;
	}
	
	if(CurrentWeaponInstance == nullptr || IsUnarmedInstance(CurrentWeaponInstance))
	{
		Player->GetWeaponComponent()->SetUnarmed();
	}
	else
	{
		Player->GetWeaponComponent()->SetWeaponWithAmmo(CurrentWeaponInstance,FindMatchingAmmo(CurrentWeaponInstance));
	}
}

void UPlayerInventoryComponent::EquipWeapon(UCPP_WeaponInstance* _WeaponInstance, bool bIsPrimary)
{
	//기존에 무기를 착용하고 있는 무기를 인벤토리에 돌려둔다
	UnEquipWeapon(bIsPrimary);
	if(bIsPrimary)
	{
		PrimaryWeaponInstance = _WeaponInstance;
	}
	else
	{
		SecondaryWeaponInstance = _WeaponInstance;
	}
	
	//비무장 상태로 전환이 아니면 인벤토리에 있는 무기를 제거하고 장비차응로 옮긴다
	if(IsUnarmedInstance(_WeaponInstance) == false)
	{
		//인벤토리에 있는 무기 제거
		RemoveItemInstanceFromInventory(_WeaponInstance);
	}
	
	//플레이어에 무기 장착
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	Player->GetWeaponComponent()->SetWeaponWithAmmo(_WeaponInstance,FindMatchingAmmo(_WeaponInstance)); //WeaponDT에 Caliber 수정을 안하면 오류남
	_WeaponInstance->SetEquipped(true);
}

void UPlayerInventoryComponent::UnEquipWeapon(UCPP_WeaponInstance* WeaponInstance)
{
	if(PrimaryWeaponInstance == WeaponInstance)
	{
		UnEquipWeapon(true);
		return;
	}
	else if(SecondaryWeaponInstance == WeaponInstance)
	{
		UnEquipWeapon(false);
		return;
	}
	Grizzly_LOG(TEXT("%s is not Equipped"),*WeaponInstance->GetWeaponData()->WeaponName);
	
}

void UPlayerInventoryComponent::UnEquipWeapon(bool bIsPrimary)
{
	UCPP_WeaponInstance* CurrentWeaponInstance = nullptr;
	if(bIsPrimary)
	{
		CurrentWeaponInstance = PrimaryWeaponInstance;
		PrimaryWeaponInstance = GetUnarmedInstance();

	}
	else
	{
		CurrentWeaponInstance = SecondaryWeaponInstance;
		SecondaryWeaponInstance = GetUnarmedInstance();
	}
	//Unarmed무장일 경우 예외처리
	if(IsUnarmedInstance(CurrentWeaponInstance) == true)
	{
		return;
	}
	
	//플레이어에 무기 해제
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	Player->GetWeaponComponent()->SetUnarmed();

	if(CurrentWeaponInstance != nullptr)
	{
		CurrentWeaponInstance->SetEquipped(false);
		AddItemToInventory(CurrentWeaponInstance);
	}
}

void UPlayerInventoryComponent::InitWeaponInstanceToUnarmedInstance()
{
	const AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	const ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	PrimaryWeaponInstance = 	Player->GetWeaponComponent()->GetUnarmedInstance();
	SecondaryWeaponInstance = 	Player->GetWeaponComponent()->GetUnarmedInstance();
}

UCPP_Ammo* UPlayerInventoryComponent::FindMatchingAmmo(const UCPP_WeaponInstance* _WeaponInstance)
{
	if(IsUnarmedInstance(_WeaponInstance))
	{
		return nullptr;
	}

	UCPP_Ammo* Ammo =  Cast<UCPP_Ammo>(*Inventory.Find(_WeaponInstance->GetAmmoName()));
	return Ammo;
}

bool UPlayerInventoryComponent::IsUnarmedInstance(const UCPP_WeaponInstance* _WeaponInstance)
{
	if(_WeaponInstance->GetWeaponData()->WeaponName.Compare(TEXT("Unarmed")) != 0)
		return false;
	return true;
}

UCPP_WeaponInstance* UPlayerInventoryComponent::GetUnarmedInstance() const
{
	const AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	const ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	UCPP_WeaponInstance* UnarmedInstance = Player->GetWeaponComponent()->GetUnarmedInstance();
	checkf(UnarmedInstance ,TEXT("UnarmedInstance is NULL"));
	return UnarmedInstance;
}



