// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine\DataTable.h>
#include <PaperSpriteComponent.h>
#include "..\Item\CPP_Item.h"
#include "InventoryComponent.generated.h"

class ACPP_ItemContainer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

//�κ��丮 ���ø����̼��� ���� ����ü
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


	// ������ �����ε� 
	bool operator==(const FItemMapData& Other) const
	{
		return Key == Other.Key && Value == Other.Value;
	}
};

//����, AI�� �������� �������� ���� �����ϴ� �Լ�
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

//���� �����̳�
USTRUCT(BlueprintType)
struct FInventoryTileRow : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryTileNode> Column;
};

//Ÿ�� ��ǥ�� Ž���� �ʿ��� ����ü
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
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void AddItemToInventory(FName _ItemName, int _Amount = 1);

	//�ν��Ͻ�
	UFUNCTION(Server,Reliable)
	void AddItemInstanceToInventory(ACPP_Item* _ItemInstance);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RemoveItemFromInventory(FName _ItemName, int _Amount = 1);

	//�ν��Ͻ�
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RemoveItemInstanceFromInventory(ACPP_Item* _ItemInstance);

	UFUNCTION(BlueprintCallable)
	void TransferItemInstance(ACPP_Item* _ItemInstance, UInventoryComponent* _TargetInventory);
private:
	UFUNCTION(Server,Reliable)
	void TransferItemInstance_Inner(ACPP_Item* _ItemInstance, UInventoryComponent* _SourceInventory, UInventoryComponent* _TargetInventory);

	UFUNCTION(Client,Reliable)
	void RequestPlayerInventoryToTransferItemInstance(UInventoryComponent* _PlayerInventory, ACPP_Item* _ItemInstance, UInventoryComponent* _SourceInventory, UInventoryComponent* _TargetInventory);

public:
	UFUNCTION(BlueprintCallable)
	ACPP_Item* FindItemFromInventory(FName _ItemName);
	

	//_HoriSize : �κ��丮�� ���� ������
	UFUNCTION(BlueprintCallable)
	TArray<FInventoryTileRow> GetInventoryTile(int _HoriSize) const;

	// �κ��丮 ������ �Ͼ �� ȣ��Ǵ� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

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
	//����,Ŭ�� �Ѵ� �����
	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	bool HasNetOwner() const;


protected:
	TMultiMap<FName,ACPP_Item*> Inventory;

	UPROPERTY(ReplicatedUsing = OnRep_NetInventory)
	TArray<FItemMapData> NetInventory;
	

};
