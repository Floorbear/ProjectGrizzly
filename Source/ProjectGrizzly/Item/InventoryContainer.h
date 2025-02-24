// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryContainer.generated.h"

UCLASS()
class PROJECTGRIZZLY_API AInventoryContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventoryContainer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	class UInventoryComponent* GetInventory() const
	{
		return Inventory;
	}
	
private:
	UPROPERTY(BlueprintReadWrite , meta = (AllowPrivateAccess))
	class UInventoryComponent* Inventory = nullptr;

};
