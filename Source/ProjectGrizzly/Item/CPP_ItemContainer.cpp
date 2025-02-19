// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_ItemContainer.h"

// Sets default values
ACPP_ItemContainer::ACPP_ItemContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	
	Tags.Add(TEXT("Interactable"));

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ACPP_ItemContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_ItemContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

