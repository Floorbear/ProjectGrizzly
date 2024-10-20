// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <Engine\DataTable.h>
#include <PaperSpriteComponent.h>
#include "CPP_Item.generated.h"




USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Price = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPaperSprite*> Tiles;
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

	//객체를 생성하고 Init함수를 또 호출하기 싫어서 만든 팩토리 함수
	UFUNCTION(BlueprintCallable)
	static UCPP_Item* CreateItem(FName _ItemRowName, int _Amount = 0);


	UFUNCTION(BlueprintCallable)
	const FItemData GetItemData()
	{
		return ItemData;
	}

	UFUNCTION(BlueprintCallable)
	void AddAmount(int _Amount)
	{
		Amount += _Amount;
	}

	UFUNCTION(BlueprintCallable)
	int GetAmount() const
	{
		return Amount;
	}
	//Private 변수는 스폰시 노출 X
	//UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (ExposeOnSpawn = true))

private:
	FItemData ItemData;

	void Init(FName _RowName);

	const UDataTable* Table;

	int Amount = 0;
};
