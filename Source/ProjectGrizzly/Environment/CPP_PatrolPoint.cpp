// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_PatrolPoint.h"

// Sets default values
ACPP_PatrolPoint::ACPP_PatrolPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);



}

// Called when the game starts or when spawned
void ACPP_PatrolPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_PatrolPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

