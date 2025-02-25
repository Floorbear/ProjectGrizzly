// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"
#include "CPP_Player.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGrizzly/Weapon/WeaponComponent.h"

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	bWantsInitializeComponent = true;
}

ACPP_PlayableCharacter* UPlayerInventoryComponent::GetPlayableCharacter() const
{
	ACPP_PlayableCharacter* PlayableCharacter = Cast<ACPP_PlayableCharacter>(GetOwner());
	check(PlayableCharacter);
	return PlayableCharacter;
}

void UPlayerInventoryComponent::TryActivateSwapWeapon(EWeaponSlot _Slot)
{
	ACPP_PlayableCharacter* PlayableCharacter = Cast<ACPP_PlayableCharacter>(GetOwner());
	check(PlayableCharacter);

	//로컬 컨트롤러에서 어빌리티 실행을 요청한다
	//이유 :무기 인스턴스의 변수 리플리케이션이 완료된 상태에서 어빌리티 활성화가 이루워져야 하기 때문
	PlayableCharacter->TryActivateSwapWeapon(_Slot);
}

void UPlayerInventoryComponent::SwapWeaponBySlot(EWeaponSlot _Slot)
{
	if(_Slot == EWeaponSlot::Primary)
	{
		SwapWeapon(true);
		return;
	}
	else if(_Slot == EWeaponSlot::Secondary)
	{
		SwapWeapon(false);
		return;
	}

	check(false);
}

void UPlayerInventoryComponent::SwapWeapon_Implementation(bool _IsPrimary)
{
	ACPP_WeaponInstance* CurrentWeaponInstance = nullptr;
	ACPP_PlayableCharacter* PlayableCharacter = Cast<ACPP_PlayableCharacter>(GetOwner());
	
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
		PlayableCharacter->GetWeaponComponent()->SetUnarmed();
	}
	else
	{
		PlayableCharacter->GetWeaponComponent()->SetWeaponWithAmmo(CurrentWeaponInstance,FindMatchingAmmo(CurrentWeaponInstance));
	}
}

void UPlayerInventoryComponent::EquipWeapon_Implementation(ACPP_WeaponInstance* _WeaponInstance, EWeaponSlot _Slot)
{
	//기존에 무기를 착용하고 있는 무기를 인벤토리에 돌려둔다
	UnEquipWeapon_Inner(_Slot);
	if(_Slot == EWeaponSlot::Primary)
	{
		PrimaryWeaponInstance = _WeaponInstance;
	}
	else
	{
		SecondaryWeaponInstance = _WeaponInstance;
	}
	
	//지금 슬롯과 일치하면 플레이어에 무기 장착
	ACPP_PlayableCharacter* PlayableCharacter = Cast<ACPP_PlayableCharacter>(GetOwner());
	if(PlayableCharacter->GetCurrentWeaponSlot() == _Slot)
	{
		TryActivateSwapWeapon(_Slot);
	}
	_WeaponInstance->SetEquipped(true);
	OnInventoryChanged.Broadcast();
}

void UPlayerInventoryComponent::UnEquipWeapon_Implementation(ACPP_WeaponInstance* WeaponInstance)
{
	if(PrimaryWeaponInstance == WeaponInstance)
	{
		UnEquipWeapon_Inner(EWeaponSlot::Primary);
		//들고있는 무기를 장착해제하면 장착 해제 어빌리티 활성화
		if(GetPlayableCharacter()->GetCurrentWeaponSlot() == EWeaponSlot::Primary)
		{
			TryActivateSwapWeapon(EWeaponSlot::Primary);
		}
		return;
	}
	else if(SecondaryWeaponInstance == WeaponInstance)
	{
		UnEquipWeapon_Inner(EWeaponSlot::Secondary);
		if(GetPlayableCharacter()->GetCurrentWeaponSlot() == EWeaponSlot::Secondary)
		{
			TryActivateSwapWeapon(EWeaponSlot::Secondary);
		}
		return;
	}
	Grizzly_LOG(TEXT("%s is not Equipped"),*WeaponInstance->GetWeaponData()->WeaponName);
	
}

void UPlayerInventoryComponent::UnEquipWeapon_Inner(EWeaponSlot _Slot)
{
	ACPP_WeaponInstance* CurrentWeaponInstance = nullptr;
	if(EWeaponSlot::Primary == _Slot)
	{
		CurrentWeaponInstance = PrimaryWeaponInstance;
		PrimaryWeaponInstance = GetUnarmedInstance();

	}
	else
	{
		CurrentWeaponInstance = SecondaryWeaponInstance;
		SecondaryWeaponInstance = GetUnarmedInstance();
	}
	
	if(!IsUnarmedInstance(CurrentWeaponInstance))
	{
		CurrentWeaponInstance->SetEquipped(false);
	}
	OnInventoryChanged.Broadcast();
}

void UPlayerInventoryComponent::InitWeaponInstanceToUnarmedInstance_Implementation()
{
	const ACPP_PlayableCharacter* PlayableCharacter = Cast<ACPP_PlayableCharacter>(GetOwner());
	PrimaryWeaponInstance = PlayableCharacter->GetWeaponComponent()->GetUnarmedInstance();
	SecondaryWeaponInstance = PlayableCharacter->GetWeaponComponent()->GetUnarmedInstance();
	UnarmedInstance = PlayableCharacter->GetWeaponComponent()->GetUnarmedInstance();
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

bool UPlayerInventoryComponent::IsCurrentWeaponSlot(EWeaponSlot _WeaponSlot) const
{
	if(GetPlayableCharacter()->GetCurrentWeaponSlot() == _WeaponSlot)
	{
		return true;
	}
	return false;
}

void UPlayerInventoryComponent::OnRep_PrimaryWeaponInstance()
{
	if (IsCurrentWeaponSlot(EWeaponSlot::Primary))
	{
		TryActivateSwapWeapon(EWeaponSlot::Primary);
	}
	OnRep_WeaponInstance();
}

void UPlayerInventoryComponent::OnRep_SecondaryWeaponInstance()
{
	if (IsCurrentWeaponSlot(EWeaponSlot::Secondary))
	{
		TryActivateSwapWeapon(EWeaponSlot::Secondary);
	}
	OnRep_WeaponInstance();
}

void UPlayerInventoryComponent::OnRep_WeaponInstance() const
{
	OnInventoryChanged.Broadcast();
}

//서버에서 호출됨
void UPlayerInventoryComponent::OnRep_OnInventoryChanged()
{
	const ACPP_PlayableCharacter* PlayableCharacter = Cast<ACPP_PlayableCharacter>(GetOwner());

	//현재 장비한 총의 탄약을 다시 확인
	ACPP_WeaponInstance* CurrentWeaponInstance = PlayableCharacter->GetWeaponComponent()->GetCurrentWeaponInstance();
	//checkf(CurrentWeaponInstance != nullptr,TEXT("CurrentWeaponInstance is NULL"));
	if(CurrentWeaponInstance == nullptr)
		return;
	PlayableCharacter->GetWeaponComponent()->SetAmmoInstance(FindMatchingAmmo(CurrentWeaponInstance));
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



