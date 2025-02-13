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
struct FItemMapData
{
	GENERATED_BODY()
public:
	FItemMapData()
	{
		
	}
	FItemMapData(FName _Key, class ACPP_Item* _Value) : Key(_Key), Value(_Value) {}
public:
	UPROPERTY()
	FName Key = "";

	UPROPERTY()
	class ACPP_Item* Value = nullptr;


	// 연산자 오버로딩 
	bool operator==(const FItemMapData& Other) const
	{
		return Key == Other.Key && Value == Other.Value;
	}
};

USTRUCT(BlueprintType)
struct FInventoryTileNode : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEmpty = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACPP_Item* Item = NULL;
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
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void AddItemToInventory(FName _ItemName, int _Amount = 1);

	//인스턴스
	UFUNCTION(Server,Reliable)
	void AddItemInstanceToInventory(ACPP_Item* _ItemInstance);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RemoveItemFromInventory(FName _ItemName, int _Amount = 1);

	//인스턴스
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RemoveItemInstanceFromInventory(ACPP_Item* _ItemInstance);
	

	//_HoriSize : 인벤토리의 가로 사이즈
	UFUNCTION(BlueprintCallable)
	TArray<FInventoryTileRow> GetInventoryTile(int _HoriSize) const;

	// 인벤토리 변경이 일어날 때 호출되는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	//--------------------------------------------------------------------------------------------------
	//										Network
	//--------------------------------------------------------------------------------------------------
private:
	UFUNCTION(Server,Reliable)
	void On_InventoryChanged();

	UFUNCTION()
	void OnRep_NetInventory();
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//서버에서만 실행
	virtual void InitializeComponent() override;

protected:
	TMultiMap<FName,ACPP_Item*> Inventory;

	UPROPERTY(ReplicatedUsing = OnRep_NetInventory)
	TArray<FItemMapData> NetInventory;
	

};
