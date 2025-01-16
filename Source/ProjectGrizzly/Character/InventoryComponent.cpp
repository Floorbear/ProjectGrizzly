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


	//아이템이 존재하면 Amount 만큼 수량증가
	// CPP_Item은 팩토리 함수로 생성되야 하니 FindOrAdd 사용 X
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
	//존재하지 않으면 새로 생성하고 인벤토리에 추가
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
	// 아이템이 존재하지 않으면 함수 종료
	if(!Inventory.Contains(_ItemName))
	{
		return nullptr;
	}
	UCPP_Item* Item = *Inventory.Find(_ItemName);

	
	Item->AddAmount(-_Amount);
	if(Item->IsEmpty())
	{
		//Map에서 아이템 제거
		Inventory.Remove(_ItemName);
	}
	OnInventoryChanged.Broadcast();
	return Item;
}

// _HoriSize : UI의 가로 칸의 수
TArray<FInventoryTileRow> UInventoryComponent::GetInventoryTile(int _HoriSize) const
{
	if (_HoriSize <= 0)
	{
		Grizzly_LOG(TEXT("%d is not over 1"), _HoriSize);
	}
	TMap<FName, UCPP_Item*> TempInventory = Inventory;
	TArray<FInventoryTileRow> Tile;

	//타일 탐색을 위한 구조체
	InventoryTileCoordinate Pos;
	Pos.MaxX = _HoriSize;
	//Grizzly_LOG(TEXT("HoriSize : %d"), _HoriSize);
	// 타일 첫번째 행 추가
	FInventoryTileRow FisrtRow;
	FisrtRow.Column.SetNum(_HoriSize);
	Tile.Add(FisrtRow);

	//복사받은 인벤토리의 아이템이 전부 타일에 들어갈 때 까지 반복
	while (!TempInventory.IsEmpty())
	{
		UCPP_Item* CurItem = TempInventory.begin().Value();
		FName CurItemName = TempInventory.begin().Key();

		FItemData ItemData = CurItem->GetItemData();
		//현재 위치가 비어있으면 아이템을 넣을 수 있는지 확인
		if (Tile[Pos.Y].Column[Pos.X].bIsEmpty)
		{
			//현재 위치에서 가로 사이즈에 맞는 아이템이 들어가는지 확인
			if (ItemData.HSize <= _HoriSize - Pos.X)
			{
				Tile[Pos.Y].Column[Pos.X].bIsEmpty = false;
				Tile[Pos.Y].Column[Pos.X].Item = CurItem;
				TempInventory.Remove(CurItemName);

				//1*1 보다 큰 아이템의 타일 크기만큼 bIsEmpty를 변경 
				if (ItemData.HSize != 1 || ItemData.VSize != 1)
				{
					for (int i = 0; i < ItemData.VSize; i++)
					{
						//탐색 좌표가 컨테이너 사이즈보다 크면 좌표만큼 세로 컨테이너 사이즈를 키운다
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
					//탐색좌표를 커다란 아이템 끝 좌표로 이동
					Pos.X += ItemData.HSize -1;
				}

				Pos.MoveNext();
				// Grizzly_LOG(TEXT("Pos X : %d"), Pos.X);
				// Grizzly_LOG(TEXT("Pos Y : %d"), Pos.Y);
				//현재 좌표가 컨테이너 사이즈보다 크면 좌표만큼 컨테이너 사이즈를 키운다
				while (Pos.Y >= Tile.Num())
				{
					FInventoryTileRow Row;
					Row.Column.SetNum(_HoriSize);
					Tile.Add(Row);
				}
			}

		}
		//빈 위치가 나올때까지 좌표를 이동한다
		else
		{
			while (!Tile[Pos.Y].Column[Pos.X].bIsEmpty)
			{
				Pos.MoveNext();
				//현재 좌표가 컨테이너 사이즈보다 크면 좌표만큼 컨테이너 사이즈를 키운다
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

