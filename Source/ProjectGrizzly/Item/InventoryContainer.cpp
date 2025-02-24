// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryContainer.h"
#include "..\Character/InventoryComponent.h"
#include "ProjectGrizzly/Character/InventoryComponent.h"

// Sets default values
AInventoryContainer::AInventoryContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetIsReplicated(true);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AInventoryContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventoryContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

