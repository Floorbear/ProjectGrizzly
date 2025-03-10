// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Crate.h"

#include "ProjectGrizzly/Character/InventoryComponent.h"
#include "ProjectGrizzly/Core/GrizzlyGameInstance.h"


// Sets default values
ACPP_Crate::ACPP_Crate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	
	Tags.Add(TEXT("Interactable"));

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetIsReplicated(true);

	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(Mesh);
	WidgetComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void ACPP_Crate::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		UGrizzlyGameInstance* Instance = Cast<UGrizzlyGameInstance>(GetWorld()->GetGameInstance());
		auto DropTable = Instance->GetDropTable();
		FRandomItemSpawnParameter RandomItemSpawnParameter;
		GetInventory()->SpawnRandomItem(DropTable,RandomItemSpawnParameter);
	}
}

// Called every frame
void ACPP_Crate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

void ACPP_Crate::ShowWidget_Implementation()
{
	WidgetComponent->SetVisibility(true);
}

void ACPP_Crate::CloseWidget_Implementation()
{
	WidgetComponent->SetVisibility(false);
}



