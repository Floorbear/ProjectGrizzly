// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "..\ProjectGrizzly.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::AddItemToInventory(FName _ItemName, int _Amount)
{
	if (_Amount <= 0)
	{
		Grizzly_LOG(TEXT("%d is not over 1"), _Amount);
	}


	//�������� �����ϸ� Amount ��ŭ ��������
	// CPP_Item�� ���丮 �Լ��� �����Ǿ� �ϴ� FindOrAdd ��� X
	if (Inventory.Contains(_ItemName))
	{
		UCPP_Item* Item = *Inventory.Find(_ItemName);
		if (Item)
		{
			Item->AddAmount(_Amount);
			OnInventoryChanged.Broadcast();
			return;
		}
	}
	//�������� ������ ���� �����ϰ� �κ��丮�� �߰�
	UCPP_Item* NewItem = UCPP_Item::CreateItem(_ItemName,_Amount);
	if (!NewItem)
	{
		Grizzly_LOG(TEXT("%s is not included in Table"), *_ItemName.ToString());
	}
	Inventory.Add(_ItemName, NewItem);
	OnInventoryChanged.Broadcast();
}

UCPP_Item* UInventoryComponent::RemoveItemFromInventory(FName _ItemName, int _Amount)
{
	// �������� �������� ������ �Լ� ����
	if(!Inventory.Contains(_ItemName))
	{
		return nullptr;
	}
	UCPP_Item* Item = *Inventory.Find(_ItemName);

	
	Item->AddAmount(-_Amount);
	if(Item->IsEmpty())
	{
		//Map���� ������ ����
		Inventory.Remove(_ItemName);
	}
	OnInventoryChanged.Broadcast();
	return Item;
}

// _HoriSize : UI�� ���� ĭ�� ��
TArray<FInventoryTileRow> UInventoryComponent::GetInventoryTile(int _HoriSize) const
{
	if (_HoriSize <= 0)
	{
		Grizzly_LOG(TEXT("%d is not over 1"), _HoriSize);
	}
	TMap<FName, UCPP_Item*> TempInventory = Inventory;
	TArray<FInventoryTileRow> Tile;

	//Ÿ�� Ž���� ���� ����ü
	InventoryTileCoordinate Pos;
	Pos.MaxX = _HoriSize;
	//Grizzly_LOG(TEXT("HoriSize : %d"), _HoriSize);
	// Ÿ�� ù��° �� �߰�
	FInventoryTileRow FisrtRow;
	FisrtRow.Column.SetNum(_HoriSize);
	Tile.Add(FisrtRow);

	//������� �κ��丮�� �������� ���� Ÿ�Ͽ� �� �� ���� �ݺ�
	while (!TempInventory.IsEmpty())
	{
		UCPP_Item* CurItem = TempInventory.begin().Value();
		FName CurItemName = TempInventory.begin().Key();

		FItemData ItemData = CurItem->GetItemData();
		//���� ��ġ�� ��������� �������� ���� �� �ִ��� Ȯ��
		if (Tile[Pos.Y].Column[Pos.X].bIsEmpty)
		{
			//���� ��ġ���� ���� ����� �´� �������� ������ Ȯ��
			if (ItemData.HSize <= _HoriSize - Pos.X)
			{
				Tile[Pos.Y].Column[Pos.X].bIsEmpty = false;
				Tile[Pos.Y].Column[Pos.X].Item = CurItem;
				TempInventory.Remove(CurItemName);

				//1*1 ���� ū �������� Ÿ�� ũ�⸸ŭ bIsEmpty�� ���� 
				if (ItemData.HSize != 1 || ItemData.VSize != 1)
				{
					for (int i = 0; i < ItemData.VSize; i++)
					{
						//Ž�� ��ǥ�� �����̳� ������� ũ�� ��ǥ��ŭ ���� �����̳� ����� Ű���
						if (Pos.Y + i >= Tile.Num())
						{
							FInventoryTileRow Row;
							Row.Column.SetNum(_HoriSize);
							Tile.Add(Row);
						}
						for (int j = 0; j < ItemData.HSize; j++)
						{
							Tile[Pos.Y + i].Column[Pos.X + j].bIsEmpty = false;
						}
					}
					//Ž����ǥ�� Ŀ�ٶ� ������ �� ��ǥ�� �̵�
					Pos.X += ItemData.HSize -1;
				}

				Pos.MoveNext();
				// Grizzly_LOG(TEXT("Pos X : %d"), Pos.X);
				// Grizzly_LOG(TEXT("Pos Y : %d"), Pos.Y);
				//���� ��ǥ�� �����̳� ������� ũ�� ��ǥ��ŭ �����̳� ����� Ű���
				while (Pos.Y >= Tile.Num())
				{
					FInventoryTileRow Row;
					Row.Column.SetNum(_HoriSize);
					Tile.Add(Row);
				}
			}

		}
		//�� ��ġ�� ���ö����� ��ǥ�� �̵��Ѵ�
		else
		{
			while (!Tile[Pos.Y].Column[Pos.X].bIsEmpty)
			{
				Pos.MoveNext();
				//���� ��ǥ�� �����̳� ������� ũ�� ��ǥ��ŭ �����̳� ����� Ű���
				while (Pos.Y >= Tile.Num())
				{
					FInventoryTileRow Row;
					Row.Column.SetNum(_HoriSize);
					Tile.Add(Row);
				}
			}
		}
	}
	return Tile;
}

