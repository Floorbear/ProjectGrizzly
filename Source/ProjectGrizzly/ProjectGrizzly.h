// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPGAbilityInputID : uint8
{
	//  None
	None			UMETA(DisplayName = "None"),
	//  Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	//  Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	//  LMB
	Shoot			UMETA(DisplayName = "Shoot"),
	//  RMB
	ADS				UMETA(DisplayName = "ADS"),
	//  R
	Reload			UMETA(DisplayName = "Reload"),

	Jump			UMETA(DisplayName = "Jump")
};