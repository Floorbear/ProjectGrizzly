// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CPP_A_PGCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadStartDelegate);
/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_A_PGCharacter : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable,BlueprintCallable,EditDefaultsOnly,Category = "Reload")
	FOnReloadStartDelegate OnReloadStartDelegate;

	void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float MoveForwardAxis = 0.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float MoveRightAxis = 0.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float RightRotationInterpo = 0.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float BendDownDegree = 0.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Weapon")
	bool bADS = false;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence")
	UAnimSequenceBase* Hip_IdleSequence;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence")
	UAnimSequenceBase* Hip_ForwardSequence;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence")
	UAnimSequenceBase* Hip_BackwardSequence;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence")
	UAnimSequenceBase* ADS_IdleSequence;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence")
	UAnimSequenceBase* ADS_ForwardSequence;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence")
	UAnimSequenceBase* ADS_BackwardSequence;

	// ------ LeftHandIK ------
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "LeftHandIK")
	FTransform LeftHandTransform;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "LeftHandIK")
	float LeftHandIKAlpha = 1.0f;
private:
	UFUNCTION()
	void AnimNotify_OnReloadEnd();
	UFUNCTION()
	void AnimNotify_OnReloadStart();

	void UpdateAxis(float _DeltaSeconds);

	void UpdateADS();

	void UpdateLeftHandIK();



};
