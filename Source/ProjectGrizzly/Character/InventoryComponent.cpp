// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "..\ProjectGrizzly.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGrizzly/Weapon/CPP_WeaponInstance.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
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

void UInventoryComponent::AddItemToInventory_Implementation(FName _ItemName, int _Amount)
{
	if (_Amount <= 0)
	{
		Grizzly_LOG(TEXT("%d is not over 1"), _Amount);
	}
	
	//아이템이 존재하면 Amount 만큼 수량증가
	// CPP_Item은 팩토리 함수로 생성되야 하니 FindOrAdd 사용 X
	if (Inventory.Contains(_ItemName))
	{
		ACPP_Item* Item = *Inventory.Find(_ItemName);
		if (Item)
		{
			//아이템이 인스턴스 모드가 아닐경우에만 Amount 추가
			if(!Item->IsInstanceMode())
			{
				Item->AddAmount(_Amount);
				OnInventoryChanged.Broadcast();
				return;
			}
		}
	}
	//존재하지 않거나 인스턴스 모드일 경우 새로 생성하고 인벤토리에 추가
	//인스턴스 모드일경우 한번에 여러개 추가하면 안됨
	ACPP_Item* NewItem = ACPP_Item::CreateItem(_ItemName,GetWorld(),_Amount,this);
	checkf(!(NewItem->IsInstanceMode() && _Amount > 1),TEXT("InstanceMode not support 1 more item creation"));
	if (!NewItem)
	{
		Grizzly_LOG(TEXT("%s is not included in Table"), *_ItemName.ToString());
	}
	Inventory.Add(_ItemName, NewItem);
	OnInventoryChanged.Broadcast();
}


void UInventoryComponent::AddItemInstanceToInventory_Implementation(ACPP_Item* _ItemInstance)
{
	Inventory.Add(_ItemInstance->GetItemData().Name,_ItemInstance);
	_ItemInstance->SetParent(this);
	OnInventoryChanged.Broadcast();
}

void UInventoryComponent::RemoveItemFromInventory_Implementation(FName _ItemName, int _Amount)
{
	// 아이템이 존재하지 않으면 함수 종료
	if(!Inventory.Contains(_ItemName))
	{
		return;
	}
	ACPP_Item* Item = *Inventory.Find(_ItemName);
	Item->AddAmount(-_Amount);
	OnInventoryChanged.Broadcast();
	return;
}

void UInventoryComponent::RemoveItemInstanceFromInventory_Implementation(ACPP_Item* _ItemInstance)
{
	// 아이템이 존재하지 않으면 함수 종료
	if(!Inventory.Contains(_ItemInstance->GetItemData().Name))
	{
		return;
	}
	Inventory.RemoveSingle(_ItemInstance->GetItemData().Name,_ItemInstance);
	_ItemInstance->MarkAsGarbage();
	OnInventoryChanged.Broadcast();
	return;
}

ACPP_Item* UInventoryComponent::FindItemFromInventory(FName _ItemName)
{
	if(!Inventory.Contains(_ItemName))
	{
		return nullptr;
	}
	return *Inventory.Find(_ItemName);
}

// _HoriSize : UI의 가로 칸의 수
TArray<FInventoryTileRow> UInventoryComponent::GetInventoryTile(int _HoriSize) const
{
	if (_HoriSize <= 0)
	{
		Grizzly_LOG(TEXT("%d is not over 1"), _HoriSize);
	}
	TMultiMap<FName, ACPP_Item*> TempInventory = Inventory;
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
	int Num = TempInventory.Num();
	while (TempInventory.Num() != 0)
	{
		ACPP_Item* CurItem = TempInventory.begin().Value();
		FName CurItemName = TempInventory.begin().Key();

		FItemData ItemData = CurItem->GetItemData();
		//현재 아이템이 렌더링할수 있는 상태가 아니면 다음으로 넘어간다
		if(!CurItem->CanRender())
		{
			TempInventory.RemoveSingle(CurItemName,CurItem);
			continue;
		}
		//현재 위치가 비어있으면 아이템을 넣을 수 있는지 확인
		if (Tile[Pos.Y].Column[Pos.X].bIsEmpty)
		{
			//현재 위치에서 가로 사이즈에 맞는 아이템이 들어가는지 확인
			if (ItemData.HSize <= _HoriSize - Pos.X)
			{
				Tile[Pos.Y].Column[Pos.X].bIsEmpty = false;
				Tile[Pos.Y].Column[Pos.X].Item = CurItem;
				TempInventory.RemoveSingle(CurItemName,CurItem);

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
			//가로사이즈가 초과해서 인벤토리에 추가할 수 없으면 컨테이너 최대 사이즈만큼 이동한다
			else
			{
				FInventoryTileRow Row;
				Row.Column.SetNum(_HoriSize);
				Tile.Add(Row);
				
				Pos.Y = Tile.Num()-1;
				Pos.X = 0;
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


void UInventoryComponent::On_InventoryChanged_Implementation()
{
	//서버
	//Inventory To NetInventory
	NetInventory.Empty();
	for(auto& i : Inventory)
	{
		NetInventory.Add({i.Key,i.Value});
	}
}

void UInventoryComponent::OnRep_NetInventory()
{
	//클라이언트
	//NetInventory To Inventory
	Inventory.Empty();
	for(auto& i : NetInventory)
	{
		Inventory.Add(i.Key,i.Value);
	}
	OnInventoryChanged.Broadcast();
}


void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent,NetInventory);
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	//서버에서 OnInventoryChanged가 호출될 때
	//클라이언트에서 OnRep_Inventory가 호출
	OnInventoryChanged.AddDynamic(this,&UInventoryComponent::On_InventoryChanged);
}

