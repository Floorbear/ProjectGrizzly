// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_InteractableObject.h"

// Sets default values
ACPP_InteractableObject::ACPP_InteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	
	Tags.Add(TEXT("Interactable"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(Mesh);
	WidgetComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void ACPP_InteractableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_InteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

void ACPP_InteractableObject::ShowWidget_Implementation()
{
	WidgetComponent->SetVisibility(true);
}

void ACPP_InteractableObject::CloseWidget_Implementation()
{
	WidgetComponent->SetVisibility(false);
}



