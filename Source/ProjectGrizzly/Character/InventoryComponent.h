// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine\DataTable.h>
#include <PaperSpriteComponent.h>
#include "..\Item\CPP_Item.h"
#include "InventoryComponent.generated.h"



USTRUCT(BlueprintType)
struct FInventoryTileNode : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEmpty = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCPP_Item* Item = NULL;
};

//이중 컨테이너
USTRUCT(BlueprintType)
struct FInventoryTileRow : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryTileNode> Column;
};

//타일 좌표를 탐색에 최적화된 구조체
struct InventoryTileCoordinate
{
	int X = 0;
	int Y = 0;

	int MaxX = 1;
	void MoveNext()
	{
		X++;
		if (X >= MaxX)
		{
			X = 0;
			Y++;
		}
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIZZLY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//태그포함이름
	UFUNCTION(BlueprintCallable)
	void AddItemToInventory(FName _ItemName, int _Amount = 1);

	//_HoriSize : 인벤토리의 가로 사이즈
	UFUNCTION(BlueprintCallable)
	TArray<FInventoryTileRow> GetInventoryTile(int _HoriSize) const;

private:
	TMap<FName,UCPP_Item*> Inventory;

};
