// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIZZLY_API IInteractable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category = "Interaction")
	void Interact(class AActor*  _Instigator);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category = "Interaction")
	bool CanInteract() const;

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category = "Interaction")
	void ShowWidget();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category = "Interaction")
	void CloseWidget();
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
