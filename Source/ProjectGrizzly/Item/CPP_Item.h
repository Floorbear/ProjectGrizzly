// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <AbilitySystemComponent.h>
#include <Engine\DataTable.h>
#include <PaperSpriteComponent.h>
#include "CPP_Item.generated.h"


UENUM(BlueprintType)
enum class EItemInteraction : uint8
{
	Equip			UMETA(DisplayName = "Equip"),
	Use			UMETA(DisplayName = "Use")
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	NoDrop,		
	Common,
	Uncommon,
	Rare,
	Unique
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NameWithOutPrefix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EItemInteraction> Interactions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Price = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> ItemEffects;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> AM_Hands_UseItem;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> AM_UseItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VSize = 1;




	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPaperSprite*> Tiles;


};


/**
 *
 */
UCLASS(BlueprintType)
class PROJECTGRIZZLY_API ACPP_Item : public AActor
{
	GENERATED_BODY()

public:
	ACPP_Item();

	//Item 생성 팩토리 함수
	UFUNCTION(BlueprintCallable)
	static ACPP_Item* CreateItem(FName _ItemRowName,UObject* _WorldContext,int _Amount = 1 , class UInventoryComponent* _Parent = nullptr);

	template <typename T>
	static T* CreateItem_Inner(FName _ItemRowName,UObject* _WorldContext , int _Amount = 1,class UInventoryComponent* _Parent  = nullptr)
	{
		UWorld* World = _WorldContext->GetWorld();
		T* NewItem = World->SpawnActor<T>();
		if(_Parent != nullptr)
		{
			AActor* InventoryActor = Cast<AActor>(_Parent->GetOwner());
			checkf(InventoryActor != nullptr,TEXT("_Parent's Owner is not Pawn"));
			NewItem->SetOwner(InventoryActor);
		}
		NewItem->Init(_ItemRowName);
		NewItem->Amount = _Amount;
		NewItem->Parent = _Parent;
		return NewItem;
	}

	UFUNCTION(BlueprintCallable)
	FItemData GetItemData() const
	{
		return ItemData;
	}

	UFUNCTION(BlueprintCallable)
	FName GetItemName() const
	{
		return ItemData.Name;
	}
	
	UFUNCTION(BlueprintCallable)
	void AddAmount(int _Amount)
	{
		Amount = FMath::Clamp(Amount + _Amount,0,9999);
		CheckConditionAndRemove();
	}
	UFUNCTION(BlueprintCallable)
	void SetAmount(int _Amount)
	{
		Amount = FMath::Clamp(_Amount,0,9999);
		CheckConditionAndRemove();
	}
	UFUNCTION(BlueprintCallable)
	int GetAmount() const
	{
		return Amount;
	}
	
	UFUNCTION(BlueprintCallable)
	bool IsEmpty() const
	{
		if(Amount == 0)
			return true;
		return false;
	}


	virtual void Init(FName _RowName);
protected:
	UPROPERTY(ReplicatedUsing = OnRep_ItemData)
	FItemData ItemData;


	UDataTable* GetItemDT() const;

	//--------------------------------------------------------------------------------------------------
	//										Amount
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool IsInstanceMode() const
	{
		return InstanceMode;
	}
protected:
	UPROPERTY(Replicated)
	int Amount = 1;

	//  Amount로 퉁칠수 없는 개별 속성이 있는 아이템 ex) 총기, 방어구
	bool InstanceMode = false;

private:
	UDataTable* ItemDT;
	//--------------------------------------------------------------------------------------------------
	//										Inventory
	//--------------------------------------------------------------------------------------------------
protected:
	UPROPERTY(Replicated , meta = (AllowPrivateAccess = true), BlueprintReadOnly)
	class UInventoryComponent* Parent = nullptr;

public:
	virtual bool CanRender() const
	{
		return true;
	}

public:
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void SetParent(class UInventoryComponent* _Parent);

	UFUNCTION(BlueprintCallable)
	class UInventoryComponent* GetParent() const
	{
		return  Parent;
	}
private:
	UFUNCTION(Server,Reliable)
	void CheckConditionAndRemove();

public:
	//--------------------------------------------------------------------------------------------------
	//										Use
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable,Category = Use)
	bool IsUsable() const;
	//--------------------------------------------------------------------------------------------------
	//										NetWork
	//--------------------------------------------------------------------------------------------------
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_ItemData(){};

	UFUNCTION(BlueprintCallable)
	virtual bool HasNetOwner() const override;
};
