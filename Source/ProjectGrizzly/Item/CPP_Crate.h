// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "CPP_Crate.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_Crate :public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Crate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	virtual bool CanInteract_Implementation() const override
	{
		return true;
	}

	//--------------------------------------------------------------------------------------------------
	//										Inventory
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	class UInventoryComponent* GetInventory() const
	{
		return Inventory;
	}
	
private:
	UPROPERTY(BlueprintReadWrite , meta = (AllowPrivateAccess))
	class UInventoryComponent* Inventory = nullptr;
	
	//--------------------------------------------------------------------------------------------------
	//										Visual
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetMesh() const
	{
		return Mesh;
	}
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= "Visual",meta=(AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh = nullptr;

	//--------------------------------------------------------------------------------------------------
	//										UI
	//--------------------------------------------------------------------------------------------------
protected:
	virtual void ShowWidget_Implementation() override;
	virtual void CloseWidget_Implementation() override;
public:
	UFUNCTION(BlueprintCallable)
	UWidgetComponent* GetWidgetComponent() const
	{
		return WidgetComponent;
	}
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= "UI",meta=(AllowPrivateAccess = true))
	UWidgetComponent* WidgetComponent = nullptr;
};
