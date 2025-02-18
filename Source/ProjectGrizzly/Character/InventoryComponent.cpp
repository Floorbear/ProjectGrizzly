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
	
	//�������� �����ϸ� Amount ��ŭ ��������
	// CPP_Item�� ���丮 �Լ��� �����Ǿ� �ϴ� FindOrAdd ��� X
	if (Inventory.Contains(_ItemName))
	{
		ACPP_Item* Item = *Inventory.Find(_ItemName);
		if (Item)
		{
			//�������� �ν��Ͻ� ��尡 �ƴҰ�쿡�� Amount �߰�
			if(!Item->IsInstanceMode())
			{
				Item->AddAmount(_Amount);
				OnInventoryChanged.Broadcast();
				return;
			}
		}
	}
	//�������� �ʰų� �ν��Ͻ� ����� ��� ���� �����ϰ� �κ��丮�� �߰�
	//�ν��Ͻ� ����ϰ�� �ѹ��� ������ �߰��ϸ� �ȵ�
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
	// �������� �������� ������ �Լ� ����
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
	// �������� �������� ������ �Լ� ����
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

// _HoriSize : UI�� ���� ĭ�� ��
TArray<FInventoryTileRow> UInventoryComponent::GetInventoryTile(int _HoriSize) const
{
	if (_HoriSize <= 0)
	{
		Grizzly_LOG(TEXT("%d is not over 1"), _HoriSize);
	}
	TMultiMap<FName, ACPP_Item*> TempInventory = Inventory;
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
	int Num = TempInventory.Num();
	while (TempInventory.Num() != 0)
	{
		ACPP_Item* CurItem = TempInventory.begin().Value();
		FName CurItemName = TempInventory.begin().Key();

		FItemData ItemData = CurItem->GetItemData();
		//���� �������� �������Ҽ� �ִ� ���°� �ƴϸ� �������� �Ѿ��
		if(!CurItem->CanRender())
		{
			TempInventory.RemoveSingle(CurItemName,CurItem);
			continue;
		}
		//���� ��ġ�� ��������� �������� ���� �� �ִ��� Ȯ��
		if (Tile[Pos.Y].Column[Pos.X].bIsEmpty)
		{
			//���� ��ġ���� ���� ����� �´� �������� ������ Ȯ��
			if (ItemData.HSize <= _HoriSize - Pos.X)
			{
				Tile[Pos.Y].Column[Pos.X].bIsEmpty = false;
				Tile[Pos.Y].Column[Pos.X].Item = CurItem;
				TempInventory.RemoveSingle(CurItemName,CurItem);

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
			//���λ���� �ʰ��ؼ� �κ��丮�� �߰��� �� ������ �����̳� �ִ� �����ŭ �̵��Ѵ�
			else
			{
				FInventoryTileRow Row;
				Row.Column.SetNum(_HoriSize);
				Tile.Add(Row);
				
				Pos.Y = Tile.Num()-1;
				Pos.X = 0;
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


void UInventoryComponent::On_InventoryChanged_Implementation()
{
	//����
	//Inventory To NetInventory
	NetInventory.Empty();
	for(auto& i : Inventory)
	{
		NetInventory.Add({i.Key,i.Value});
	}
}

void UInventoryComponent::OnRep_NetInventory()
{
	//Ŭ���̾�Ʈ
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
	//�������� OnInventoryChanged�� ȣ��� ��
	//Ŭ���̾�Ʈ���� OnRep_Inventory�� ȣ��
	OnInventoryChanged.AddDynamic(this,&UInventoryComponent::On_InventoryChanged);
}

