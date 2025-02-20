// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <Engine\DataTable.h>
#include <PaperSpriteComponent.h>
#include "CPP_Item.generated.h"


UENUM(BlueprintType)
enum class EItemInteraction : uint8
{
	Equip			UMETA(DisplayName = "Equip"),
	Use			UMETA(DisplayName = "Use")
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
	int HSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Price = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPaperSprite*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EItemInteraction> Interactions;
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
			AController* Controller = Cast<AController>(_Parent->GetOwner());
			checkf(Controller != nullptr,TEXT("_Parent's Owner is not Controller"));
			NewItem->SetOwner(Controller);
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
	UPROPERTY(Replicated)
	class UInventoryComponent* Parent = nullptr;

public:
	virtual bool CanRender() const
	{
		return true;
	}

public:
	UFUNCTION(Server,Reliable)
	void SetParent(class UInventoryComponent* _Parent);
	// Amount == 0 을 만족하면 부모로부터 이 아이템을 삭제하라고 하는 함수
private:
	UFUNCTION(Server,Reliable)
	void CheckConditionAndRemove();

public:

	//--------------------------------------------------------------------------------------------------
	//										NetWork
	//--------------------------------------------------------------------------------------------------
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_ItemData(){};
};
