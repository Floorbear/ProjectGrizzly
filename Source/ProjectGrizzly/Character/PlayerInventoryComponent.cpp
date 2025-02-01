// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"
#include "CPP_Player.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
#include "GrizzlyPC.h"
#include "ProjectGrizzly/Weapon/WeaponComponent.h"

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InitUnarmedInstance();

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
	
	if(CurrentWeaponInstance == nullptr)
	{
		Player->GetWeaponComponent()->SetUnarmed();
	}
	else
	{
		Player->GetWeaponComponent()->SetWeapon(CurrentWeaponInstance);
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
		FString WeaponName(TEXT("Weapon_"));
		WeaponName.Append(_WeaponInstance->GetWeaponData()->WeaponName);
		RemoveItemInstanceFromInventory(_WeaponInstance);
	}
	
	//플레이어에 무기 장착
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	Player->GetWeaponComponent()->SetWeapon(_WeaponInstance);

	
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
		//CurrentWeaponInstance->AddAmount(1);
		AddItemToInventory(CurrentWeaponInstance);
	}
}

void UPlayerInventoryComponent::InitUnarmedInstance()
{
	const AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	const ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	PrimaryWeaponInstance = 	Player->GetWeaponComponent()->GetUnarmedInstance();
	SecondaryWeaponInstance = 	Player->GetWeaponComponent()->GetUnarmedInstance();
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


