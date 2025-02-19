// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "ProjectGrizzly/Character/InventoryComponent.h"
#include "CPP_ItemContainer.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_ItemContainer : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_ItemContainer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetInventory() const
	{
		return Inventory;
	}
protected:
	virtual bool CanInteract_Implementation() const override
	{
		return true;
	}
private:
	UPROPERTY()
	UInventoryComponent* Inventory = nullptr;

};
