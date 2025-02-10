// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine\DataTable.h>
#include <PaperSpriteComponent.h>
#include "..\Item\CPP_Item.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

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

//���� �����̳�
USTRUCT(BlueprintType)
struct FInventoryTileRow : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryTileNode> Column;
};

//Ÿ�� ��ǥ�� Ž���� ����ȭ�� ����ü
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

	//�±������̸�
	UFUNCTION(BlueprintCallable)
	void AddItemToInventory(FName _ItemName, int _Amount = 1);

	//�ν��Ͻ�
	void AddItemToInventory(UCPP_Item* _ItemInstance);

	UFUNCTION(BlueprintCallable)
	UCPP_Item* RemoveItemFromInventory(FName _ItemName, int _Amount = 1);

	//�ν��Ͻ�
	UFUNCTION(BlueprintCallable)
	UCPP_Item* RemoveItemInstanceFromInventory(UCPP_Item* _ItemInstance);
	

	//_HoriSize : �κ��丮�� ���� ������
	UFUNCTION(BlueprintCallable)
	TArray<FInventoryTileRow> GetInventoryTile(int _HoriSize) const;

	// �κ��丮 ������ �Ͼ �� ȣ��Ǵ� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;


protected:
	TMultiMap<FName,UCPP_Item*> Inventory;

};
