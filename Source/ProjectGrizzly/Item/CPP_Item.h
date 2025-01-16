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
class PROJECTGRIZZLY_API UCPP_Item : public UObject
{
	GENERATED_BODY()

public:
	UCPP_Item();

	//Item 생성 팩토리 함수
	UFUNCTION(BlueprintCallable)
	static UCPP_Item* CreateItem(FName _ItemRowName, int _Amount = 0);

	template <typename T>
	static T* CreateItem_Inner(FName _ItemRowName, int _Amount = 0)
	{
		T* NewItem = NewObject<T>();
		NewItem->Init(_ItemRowName);
		NewItem->Amount = _Amount;
		return NewItem;
	}

	UFUNCTION(BlueprintCallable)
	const FItemData GetItemData()
	{
		return ItemData;
	}

	UFUNCTION(BlueprintCallable)
	void AddAmount(int _Amount)
	{
		Amount = FMath::Clamp(Amount + _Amount,0,9999);
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
	FItemData ItemData;


	UDataTable* GetItemDT() const;

	int Amount = 0;

private:
	UDataTable* ItemDT = nullptr;
};
