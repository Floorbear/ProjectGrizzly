// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine\DataTable.h>
#include <PaperSpriteComponent.h>
#include "..\Item\CPP_Item.h"
#include "ProjectGrizzly/Weapon/WeaponData.h"
#include "InventoryComponent.generated.h"

class ACPP_ItemContainer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

//인벤토리 리플리케이션을 위한 구조체
USTRUCT(BlueprintType)
struct FItemMapData
{
	GENERATED_BODY()
public:
	FItemMapData()
	{
		
	}
	FItemMapData(FName _Key, class ACPP_Item* _Value) : Key(_Key), Value(_Value) {}
	FItemMapData(FName _Key, int _Amount) : Key(_Key), Amount(_Amount) {}
public:
	UPROPERTY()
	FName Key = "";

	int Amount = 1;

	UPROPERTY()
	class ACPP_Item* Value = nullptr;


	// 연산자 오버로딩 
	bool operator==(const FItemMapData& Other) const
	{
		return Key == Other.Key && Value == Other.Value;
	}
};

//
USTRUCT(BlueprintType)
struct FInventoryEntry
{
	GENERATED_BODY()
public:
	FInventoryEntry()
	{
		
	}

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ItemName = "";

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Amount = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EWeaponSlot EquippedSlot = EWeaponSlot::NoEquip;
};

USTRUCT(BlueprintType)
struct FInventoryData
{
	GENERATED_BODY()
public:
	FInventoryData()
	{
		
	}

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName InventoryName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FInventoryEntry> InventoryData;

	bool operator== (const FInventoryData& _Data) const
	{
		if(_Data.InventoryName == this->InventoryName)
			return true;
		return false;
	}
};

//상자, AI에 아이템을 랜덤으로 스폰 설정하는 함수
USTRUCT(BlueprintType)
struct FRandomItemSpawnParameter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EItemRarity MaxSpawnRarity = EItemRarity::Unique;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MinSpawnCount = 1;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MaxSpawnCount = 3;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MinAmmoCount = 1;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MaxAmmoCount = 15;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FName> IgnoreSpawnType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FName> MustSpawnType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FName,float> SpawnRateOverride;
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

//타일 좌표를 탐색에 필요한 구조체
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
	
	void AddItemToInventory_Inner(FInventoryEntry& Entry);

	//인스턴스
	UFUNCTION(Server,Reliable)
	void AddItemInstanceToInventory(ACPP_Item* _ItemInstance);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RemoveItemFromInventory(FName _ItemName, int _Amount = 1);

	//인스턴스
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RemoveItemInstanceFromInventory(ACPP_Item* _ItemInstance);

	UFUNCTION(BlueprintCallable)
	void TransferItemInstance(ACPP_Item* _ItemInstance, UInventoryComponent* _TargetInventory);
private:
	bool IsEmptyTileRange(int _PosXStart, int _PosXEnd, int _PosY, TArray<FInventoryTileRow> _Tile) const;
	UFUNCTION(Server,Reliable)
	void TransferItemInstance_Inner(ACPP_Item* _ItemInstance, UInventoryComponent* _SourceInventory, UInventoryComponent* _TargetInventory);

	UFUNCTION(Client,Reliable)
	void RequestPlayerInventoryToTransferItemInstance(UInventoryComponent* _PlayerInventory, ACPP_Item* _ItemInstance, UInventoryComponent* _SourceInventory, UInventoryComponent* _TargetInventory);

public:
	UFUNCTION(BlueprintCallable)
	ACPP_Item* FindItemFromInventory(FName _ItemName);
	

	//_HoriSize : 인벤토리의 가로 사이즈
	UFUNCTION(BlueprintCallable)
	TArray<FInventoryTileRow> GetInventoryTile(int _HoriSize) const;
	

	// 인벤토리 변경이 일어날 때 호출되는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	// 인벤토리 자체가 재할당 될 때 호출되는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventorySet;

	//--------------------------------------------------------------------------------------------------
	//										RandomItem
	//--------------------------------------------------------------------------------------------------
public:
	void SpawnRandomItem(TMap<FName,TArray<FItemData*>> _DropTable,FRandomItemSpawnParameter _Parameter);
	static FItemData* GetRandomItemData(TArray<FItemData*> _DropList);
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
	//서버,클라 둘다 실행됨
	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	bool HasNetOwner() const;

	//--------------------------------------------------------------------------------------------------
	//										Interface for Instance
	//--------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void SetInventory(FInventoryData _InventoryData);
	UFUNCTION(BlueprintCallable)
	FInventoryData ToInventoryData(FName _InventoryName) const;

	//--------------------------------------------------------------------------------------------------
	//										Core
	//--------------------------------------------------------------------------------------------------
protected:
	TMultiMap<FName,ACPP_Item*> Inventory;

	UPROPERTY(ReplicatedUsing = OnRep_NetInventory)
	TArray<FItemMapData> NetInventory;
	

};


