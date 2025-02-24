// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InventoryContainer.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "CPP_InteractableObject.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_InteractableObject : public AInventoryContainer, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_InteractableObject();

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
