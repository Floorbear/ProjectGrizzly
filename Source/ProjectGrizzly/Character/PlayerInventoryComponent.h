// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_PlayableCharacter.h"
#include "InventoryComponent.h"
#include "..\Weapon/CPP_WeaponInstance.h"
#include "..\Weapon/CPP_Ammo.h"
#include "PlayerInventoryComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UPlayerInventoryComponent : public UInventoryComponent
{
	GENERATED_BODY()

	void BeginPlay() override;
	//--------------------------------------------------------------------------------------------------
	//										Inner Interface
	//--------------------------------------------------------------------------------------------------
private:
	ACPP_PlayableCharacter* GetPlayableCharacter() const; 
	//--------------------------------------------------------------------------------------------------
	//										Weapon
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	void TryActivateSwapWeapon(EWeaponSlot _Slot);
	//It means After Function EquipWeapon is called, Draw the equipped Weapon
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void SwapWeapon(bool _IsPrimary);

	void SwapWeaponBySlot(EWeaponSlot _Slot);
	
	// bIsPrimary : Main Weapon 
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void EquipWeapon(ACPP_WeaponInstance* _WeaponInstance, EWeaponSlot _Slot);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void UnEquipWeapon(ACPP_WeaponInstance* WeaponInstance);
	
	void UnEquipWeapon_Inner(EWeaponSlot _Slot);

public:
	UFUNCTION(Server,reliable)
	void InitWeaponInstanceToUnarmedInstance();

	//�κ��丮�� �ν��Ͻ��� ���� �ʱ�ȭ�� �� ����
	UFUNCTION(Server,reliable)
	void SetWeaponInstanceToSlot(ACPP_WeaponInstance* _Weapon,EWeaponSlot _Slot);
private:
	//
	ACPP_Ammo* FindMatchingAmmo(const ACPP_WeaponInstance* _WeaponInstance);
	//���� ���¸� Unarmed Instance�� �Ҵ��
	UPROPERTY(ReplicatedUsing = OnRep_PrimaryWeaponInstance)
	ACPP_WeaponInstance* PrimaryWeaponInstance = nullptr;
	
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeaponInstance)
	ACPP_WeaponInstance* SecondaryWeaponInstance = nullptr;
	
	UPROPERTY(Replicated)
	ACPP_WeaponInstance* UnarmedInstance = nullptr;

	//--------------------------------------------------------------------------------------------------
	//										Weapon - Interface
	//--------------------------------------------------------------------------------------------------
public:
	ACPP_WeaponInstance* GetWeaponInstanceFromSlot(EWeaponSlot _Slot) const
	{
		if(_Slot == EWeaponSlot::Primary)
			return PrimaryWeaponInstance;
		else if(_Slot == EWeaponSlot::Secondary)
			return SecondaryWeaponInstance;

		checkf(false,TEXT("_Slot is invaild"));
		return  nullptr;
	}
public:
	UFUNCTION(BlueprintCallable)
	bool IsUnarmedInstance(const ACPP_WeaponInstance* _WeaponInstance) const;

	UFUNCTION(BlueprintCallable)
	ACPP_WeaponInstance* GetUnarmedInstance() const;
	bool IsCurrentWeaponSlot(EWeaponSlot _WeaponSlot) const;
	
	UFUNCTION(BlueprintCallable)
	ACPP_WeaponInstance* FindWeaponInstanceFromSlot(EWeaponSlot _Slot) const;
	//--------------------------------------------------------------------------------------------------
	//										Interface
	//--------------------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------------------
	//										Network
	//--------------------------------------------------------------------------------------------------
private:
	UFUNCTION()
	void OnRep_PrimaryWeaponInstance();
	UFUNCTION()
	void OnRep_SecondaryWeaponInstance();
	UFUNCTION()
	void OnRep_WeaponInstance() const;
	UFUNCTION()
	void OnRep_OnInventoryChanged();

	//�������� ȣ���
	//������ OnInventoryChanged�� ���ε�
	UFUNCTION()
	void CheckEquippedWeaponInstance();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	virtual void InitializeComponent() override;


	//Return PlayerControllers[0]->Inventory
	//Can Only Call in Local Client
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "_WorldContext"))
	static UPlayerInventoryComponent* GetPlayerInventoryComponent(UObject* _WorldContext);
	
};
