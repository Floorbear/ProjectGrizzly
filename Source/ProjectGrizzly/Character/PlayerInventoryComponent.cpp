// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"
#include "CPP_Player.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
#include "GrizzlyPC.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGrizzly/Weapon/WeaponComponent.h"

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	bWantsInitializeComponent = true;
}
	
void UPlayerInventoryComponent::DrawWeapon_Implementation(bool _IsPrimary)
{
	ACPP_WeaponInstance* CurrentWeaponInstance = nullptr;
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

void UPlayerInventoryComponent::EquipWeapon_Implementation(ACPP_WeaponInstance* _WeaponInstance, bool bIsPrimary)
{
	//������ ���⸦ �����ϰ� �ִ� ���⸦ �κ��丮�� �����д�
	UnEquipWeapon(bIsPrimary);
	if(bIsPrimary)
	{
		PrimaryWeaponInstance = _WeaponInstance;
	}
	else
	{
		SecondaryWeaponInstance = _WeaponInstance;
	}
	
	
	//�÷��̾ ���� ����
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	Player->GetWeaponComponent()->SetWeaponWithAmmo(_WeaponInstance,FindMatchingAmmo(_WeaponInstance)); //WeaponDT�� Caliber ������ ���ϸ� ������
	_WeaponInstance->SetEquipped(true);
	OnInventoryChanged.Broadcast();
}

void UPlayerInventoryComponent::UnEquipWeapon_Implementation(ACPP_WeaponInstance* WeaponInstance)
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
	ACPP_WeaponInstance* CurrentWeaponInstance = nullptr;
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
	//Unarmed������ ��� ����ó��
	if(IsUnarmedInstance(CurrentWeaponInstance) == true)
	{
		return;
	}
	
	//�÷��̾ ���� ����
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	Player->GetWeaponComponent()->SetUnarmed();

	if(CurrentWeaponInstance != nullptr)
	{
		CurrentWeaponInstance->SetEquipped(false);
		//AddItemToInventory(CurrentWeaponInstance);
	}
	OnInventoryChanged.Broadcast();
}

void UPlayerInventoryComponent::InitWeaponInstanceToUnarmedInstance_Implementation()
{
	const AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	const ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());
	PrimaryWeaponInstance = Player->GetWeaponComponent()->GetUnarmedInstance();
	SecondaryWeaponInstance = Player->GetWeaponComponent()->GetUnarmedInstance();
	UnarmedInstance = Player->GetWeaponComponent()->GetUnarmedInstance();
}

ACPP_Ammo* UPlayerInventoryComponent::FindMatchingAmmo(const ACPP_WeaponInstance* _WeaponInstance)
{
	if(IsUnarmedInstance(_WeaponInstance))
	{
		return nullptr;
	}

	if(!Inventory.Contains(_WeaponInstance->GetAmmoName()))
	{
		return nullptr;
	}

	ACPP_Ammo* Ammo =  Cast<ACPP_Ammo>(*Inventory.Find(_WeaponInstance->GetAmmoName()));
	return Ammo;
}

bool UPlayerInventoryComponent::IsUnarmedInstance(const ACPP_WeaponInstance* _WeaponInstance) const
{
	// if(_WeaponInstance->GetWeaponData()->WeaponName.Compare(TEXT("Unarmed")) != 0)
	// 	return false;
	if(_WeaponInstance != UnarmedInstance)
		return false;
	return true;
}

ACPP_WeaponInstance* UPlayerInventoryComponent::GetUnarmedInstance() const
{
	return UnarmedInstance;
}

void UPlayerInventoryComponent::OnRep_WeaponInstance() const
{
	OnInventoryChanged.Broadcast();
}

//�������� ȣ���
void UPlayerInventoryComponent::OnRep_OnInventoryChanged()
{
	const AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetOwner());
	const ACPP_Player* Player = Cast<ACPP_Player>(PC->GetPawn());

	//���� ����� ���� ź���� �ٽ� Ȯ��
	ACPP_WeaponInstance* CurrentWeaponInstance = Player->GetWeaponComponent()->GetCurrentWeaponInstance();
	checkf(CurrentWeaponInstance != nullptr,TEXT("CurrentWeaponInstance is NULL"));
	Player->GetWeaponComponent()->SetAmmoInstance(FindMatchingAmmo(CurrentWeaponInstance));
}

void UPlayerInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerInventoryComponent,PrimaryWeaponInstance);
	DOREPLIFETIME(UPlayerInventoryComponent,SecondaryWeaponInstance);
	DOREPLIFETIME(UPlayerInventoryComponent,UnarmedInstance);
}

void UPlayerInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	OnInventoryChanged.AddDynamic(this,&UPlayerInventoryComponent::OnRep_OnInventoryChanged);
}



