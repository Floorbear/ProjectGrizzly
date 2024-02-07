// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "CPP_GrizzlyPS.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API ACPP_GrizzlyPS : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACPP_GrizzlyPS();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UGrizzlyAttributeSet* GetAttributeSet() const;

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Ability",meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemCompoent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Ability",meta = (AllowPrivateAccess = "true"))
	class UGrizzlyAttributeSet* AttributeSet;

};
