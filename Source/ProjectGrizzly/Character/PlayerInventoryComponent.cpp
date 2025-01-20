// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"
#include "CPP_Player.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
#include "GrizzlyPC.h"
#include "ProjectGrizzly/Weapon/WeaponComponent.h"

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

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
	if(bIsPrimary)
	{
		PrimaryWeaponInstance = _WeaponInstance;
		// Grizzly_LOG(TEXT("%s is Equipped"),*_WeaponInstance->GetWeaponData()->WeaponName)
	}
	else
		SecondaryWeaponInstance = _WeaponInstance;

	
	//���� ���·� ��ȯ�� �ƴϸ� �κ��丮�� �ִ� ���⸦ �����ϰ� ��������� �ű��
	if(_WeaponInstance->GetWeaponData()->WeaponName.Compare(TEXT("Unarmed")) != 0)
	{
		//�κ��丮�� �ִ� ���� ����
		FString WeaponName(TEXT("Weapon_"));
		WeaponName.Append(_WeaponInstance->GetWeaponData()->WeaponName);
		RemoveItemFromInventory(*WeaponName);
	}
	
	
	//�÷��̾ ���� ����
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
		PrimaryWeaponInstance = nullptr;

	}
	else
	{
		CurrentWeaponInstance = SecondaryWeaponInstance;
		SecondaryWeaponInstance = nullptr;
	}
	
	//�÷��̾ ���� ����
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	Player->GetWeaponComponent()->SetUnarmed();

	if(CurrentWeaponInstance != nullptr)
	{
		AddItemToInventory(CurrentWeaponInstance);
	}
}


