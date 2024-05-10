// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ProjectGrizzly\ProjectGrizzly.h"
#include "GrizzlyAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UGrizzlyAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EPGAbilityInputID InputID = EPGAbilityInputID::None;

protected:
	UFUNCTION(BlueprintCallable)
	class ACPP_PlayableCharacter* GetCharacter() const;

	UFUNCTION(BlueprintCallable)
	class UWeaponComponent* GetWeaponComponent() const;

	UFUNCTION(BlueprintCallable)
	bool IsAIControl() const;
};
