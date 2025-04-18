// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "GrizzlyPC.h"
#include "..\ProjectGrizzly.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGrizzly/Core/GrizzlyGameInstance.h"
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

	//오너가 폰이면 인벤토리 변경 이벤트 바인드
	if(const APawn* OwnerPawn = Cast<APawn>(GetOwner());OwnerPawn)
	{
		//SimulatedProxy가 이벤트 바인드 되는 것 방지
		if(OwnerPawn->HasAuthority() || OwnerPawn->IsLocallyControlled())
		{
			OnInventoryChanged.AddDynamic(this,&UInventoryComponent::On_InventoryChanged);
		}
	}
	else
	{
		//상자 같은 오브젝트의 이벤트 바인드
		//서버에 있는 오브젝트반 이벤트를 바인드
		if(GetOwner()->HasAuthority())
		{
			OnInventoryChanged.AddDynamic(this,&UInventoryComponent::On_InventoryChanged);
		}
	}
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::AddItemToInventory_Inner(FInventoryEntry& Entry)
{
	int _Amount = Entry.Amount;
	FName _ItemName = Entry.ItemName;
	
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

	//무기면 장착 데이터를 추가해줌
	if(ACPP_WeaponInstance* WeaponInstance = Cast<ACPP_WeaponInstance>(NewItem);WeaponInstance)
	{
		WeaponInstance->SetEquipped(Entry.EquippedSlot);
	}
	Inventory.Add(_ItemName, NewItem);
	OnInventoryChanged.Broadcast();
}


void UInventoryComponent::AddItemToInventory_Implementation(FName _ItemName, int _Amount)
{
	FInventoryEntry Entry;
	Entry.ItemName = _ItemName;
	Entry.Amount = _Amount;
	AddItemToInventory_Inner(Entry);
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
}

void UInventoryComponent::TransferItemInstance_Inner_Implementation(ACPP_Item* _ItemInstance,
               UInventoryComponent* _SourceInventory, UInventoryComponent* _TargetInventory)
{
	check(_SourceInventory);
	check(_TargetInventory);
	check(_ItemInstance->GetParent());

	_ItemInstance->SetParent(_TargetInventory);
	if(_ItemInstance->IsInstanceMode())
	{
		_TargetInventory->AddItemInstanceToInventory(_ItemInstance);
	}
	else
	{
		//아이템이 TargetInventory에 이미 존재하는경우 이미 존재하는 아이템의 Amount만 증가
		if(ACPP_Item* FindItem =_TargetInventory->FindItemFromInventory(_ItemInstance->GetItemName());
			FindItem)
		{
			FindItem->AddAmount(_ItemInstance->GetAmount());
		}
		else
		{
			//아이템이 TargetInventory에 존재하지 않기 때문에 그대로 옮김
			_TargetInventory->AddItemInstanceToInventory(_ItemInstance);
		}
		
	}
	_SourceInventory->Inventory.RemoveSingle(_ItemInstance->GetItemName(),_ItemInstance);
		
	_SourceInventory->OnInventoryChanged.Broadcast();
	_TargetInventory->OnInventoryChanged.Broadcast();
}

void UInventoryComponent::RequestPlayerInventoryToTransferItemInstance_Implementation(UInventoryComponent* _PlayerInventory,
	ACPP_Item* _ItemInstance, UInventoryComponent* _SourceInventory, UInventoryComponent* _TargetInventory)
{
	if(!_PlayerInventory->GetOwner()->HasNetOwner())
	{
		return;
	}

	_PlayerInventory->TransferItemInstance_Inner(_ItemInstance,_SourceInventory,_TargetInventory);
}

void UInventoryComponent::TransferItemInstance(ACPP_Item* _ItemInstance,
	UInventoryComponent* _TargetInventory)
{
	if(!this->GetOwner()->HasNetOwner())
	{
		UInventoryComponent* PlayerInventory =Cast<AGrizzlyPC>(GetWorld()->GetFirstPlayerController())->GetInventoryComponent();
		RequestPlayerInventoryToTransferItemInstance(PlayerInventory,_ItemInstance,
			this,_TargetInventory);
	}
	else
	{
		TransferItemInstance_Inner(_ItemInstance,this,_TargetInventory);
	}
	
}

bool UInventoryComponent::IsEmptyTileRange(int _PosXStart, int _PosXEnd, int _PosY, TArray<FInventoryTileRow> _Tile) const
{
	bool Result = true;
	for(int i = _PosXStart; i <= _PosXEnd; i++)
	{
		if(i >= _Tile[_PosY].Column.Num())
		{
			return false;
		}
		if(!_Tile[_PosY].Column[i].bIsEmpty)
		{
			return false;
		}
	}

	return true;
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
		if(!CurItem)
		{
			return Tile;
		}

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
			if (ItemData.HSize <= _HoriSize - Pos.X && IsEmptyTileRange(Pos.X,Pos.X + ItemData.HSize,Pos.Y,Tile))
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

void UInventoryComponent::SpawnRandomItem(TMap<FName, TArray<FItemData*>> _DropTable,
	FRandomItemSpawnParameter _Parameter)
{
	//필수 아이템 스폰
	for(int i =0; i < _Parameter.MustSpawnType.Num();i++)
	{
		TArray<FItemData*> DropList = _DropTable[_Parameter.MustSpawnType[i]];
		FItemData* ItemData = GetRandomItemData(DropList);
		int SpawnAmount = 1;
		if(ItemData->Type == TEXT("Ammo"))
		{
			SpawnAmount =  FMath::RandRange(_Parameter.MinAmmoCount,_Parameter.MaxAmmoCount);
		}
		AddItemToInventory(ItemData->Name,SpawnAmount);
		_Parameter.IgnoreSpawnType.Add(ItemData->Type);
	}

	//드랍 테이블 구성
	TArray<FItemData*> DropList;
	for(auto i : _DropTable)
	{
		if(_Parameter.IgnoreSpawnType.Contains(i.Key))
			continue;

		//스폰 레이트 설정
		float SpawnRate = 1.0f;
		if(_Parameter.SpawnRateOverride.Contains(i.Key))
		{
			SpawnRate = _Parameter.SpawnRateOverride[i.Key];
		}

		//각 아이템을 드랍 리스트에 넣음
		for(auto j : i.Value)
		{
			const int MaxRarity = 5;
			//레어도에 따른 가중치 설정 후 가중치 만큼 드랍리스트에 넣기
			float fWeight = MaxRarity - static_cast<int>(j->ItemRarity);
			int Weight = FMath::CeilToInt(fWeight * SpawnRate);
			//리스트에 넣기
			for(int k =0; k < Weight; k++)
			{
				DropList.Add(j);
			}
		}
	}

	//드랍 리스트에 있는 element 중 랜덤 아이템 스폰
	int SpawnCount = FMath::RandRange(_Parameter.MinSpawnCount,_Parameter.MaxSpawnCount);
	for(int i = 0; i < SpawnCount; i++)
	{
		FItemData* ItemData = GetRandomItemData(DropList);
		//DropList에 아이템이 더이상 존재하지 않는경우
		if(!ItemData)
			break;
		int SpawnAmount = 1;
		if(ItemData->Type == TEXT("Ammo"))
		{
			SpawnAmount =  FMath::RandRange(_Parameter.MinAmmoCount,_Parameter.MaxAmmoCount);
		}
		AddItemToInventory(ItemData->Name,SpawnAmount);
		DropList.Remove(ItemData);
	}
}

FItemData* UInventoryComponent::GetRandomItemData(TArray<FItemData*> _DropList)
{
	if(_DropList.IsEmpty())
		return nullptr;

	int SpawnNum = FMath::RandRange(0,_DropList.Num()-1);
	return _DropList[SpawnNum];
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
		if(!i.Value)
			continue;
		i.Value->SetParent(this);
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
	
	// 아이템박스 같은 넷오너가 없는 액터는 바인드하지 않는다
}

bool UInventoryComponent::HasNetOwner() const
{
	if(!GetOwner())
		return false;

	return GetOwner()->HasNetOwner();
}

void UInventoryComponent::RequestPlayerInventoryToAddAmount_ServerRPC_Implementation(ACPP_Item* _Item, int Amount)
{
	_Item->AddAmount(Amount);
}

void UInventoryComponent::RequestPlayerInventoryToAddAmount_Implementation(ACPP_Item* _Item, int Amount)
{
	//소유권이 없는 액터에서 호출시 소유권이 있는 액터에서 호출되도록 함
	if(!HasNetOwner())
	{
		AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetWorld()->GetFirstPlayerController());
		PC->GetInventoryComponent()->RequestPlayerInventoryToAddAmount(_Item,Amount);
		return;
	}
	//소유권이 있는 액터
	else
	{
		//Item의 Amount를 설정하는 RPC
		RequestPlayerInventoryToAddAmount_ServerRPC(_Item,Amount);
		return;
	}
	
}

void UInventoryComponent::SetInventory_Implementation(FInventoryData _InventoryData)
{
	Inventory.Empty();
	for(auto& i : _InventoryData.InventoryData)
	{
		AddItemToInventory_Inner(i);
		//AddItemToInventory(i.ItemName,i.Amount);
		// i.Value->SetParent(this);
		// Inventory.Add(i.Key,i.Value);
	}
	
	OnInventoryChanged.Broadcast();
}

FInventoryData UInventoryComponent::ToInventoryData(FName _InventoryName) const
{
	FInventoryData ReturnInventory;
	ReturnInventory.InventoryName = _InventoryName;
	for(auto& i : Inventory)
	{
		FInventoryEntry NewEntry;
		NewEntry.ItemName = i.Value->GetItemName();
		NewEntry.Amount = i.Value->GetAmount();

		if(ACPP_WeaponInstance* WeaponInstance = Cast<ACPP_WeaponInstance>(i.Value);WeaponInstance)
		{
			NewEntry.EquippedSlot = WeaponInstance->GetEquippedSlot();
		}
		ReturnInventory.InventoryData.Add(NewEntry);
	}
	return ReturnInventory;
}

