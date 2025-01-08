// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "..\Weapon/\WeaponData.h"
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
	UCPP_A_PGCharacter();
	UPROPERTY(BlueprintAssignable, BlueprintCallable, EditDefaultsOnly, Category = "Reload")
	FOnReloadStartDelegate OnReloadStartDelegate;

	void NativeUpdateAnimation(float DeltaSeconds) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MoveForwardAxis = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MoveRightAxis = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float RightRotationInterpo = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float BendDownDegree = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool bADS = false;

	//----------------------------------------------------------------------------------------------------
	//											AnimSequence
	//----------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	void SetAnimMode(EWeaponType _WeaponType);

	EWeaponType CurrentWeaponType = EWeaponType::No;
	struct GrizzlyAnimSequence
	{
		UAnimSequenceBase* Hip_IdleSequence = nullptr;
		UAnimSequenceBase* Hip_ForwardSequence = nullptr;
		UAnimSequenceBase* Hip_RunSequence= nullptr;
		UAnimSequenceBase* Hip_BackwardSequence= nullptr;
		UAnimSequenceBase* ADS_IdleSequence= nullptr;
		UAnimSequenceBase* ADS_ForwardSequence= nullptr;
		UAnimSequenceBase* ADS_BackwardSequence= nullptr;
		UAnimSequenceBase* Crouch_IdleSequence= nullptr;
		UAnimSequenceBase* Crouch_ForwardSequence= nullptr;
		UAnimSequenceBase* Crouch_BackwardSequence= nullptr;
	};
	TMap<EWeaponType,GrizzlyAnimSequence> AnimSequenceMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* Hip_IdleSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* Hip_ForwardSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* Hip_RunSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* Hip_BackwardSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* ADS_IdleSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* ADS_ForwardSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* ADS_BackwardSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* Crouch_IdleSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* Crouch_ForwardSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimSequence")
	UAnimSequenceBase* Crouch_BackwardSequence;


	// ------ LeftHandIK ------
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LeftHandIK")
	FTransform LeftHandTransform;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LeftHandIK")
	float LeftHandIKAlpha = 1.0f;
private:
	UFUNCTION()
	void AnimNotify_OnReloadEnd();
	UFUNCTION()
	void AnimNotify_OnReloadStart();

	void UpdateAxis(float _DeltaSeconds);

	void UpdateADS();

	void UpdateLeftHandIK();

	void UpdateLeaning(float _DeltaSeconds);


	// ----------------------------------
	//				Leaning
	// ----------------------------------
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Leaning", meta = (AllowPrivateAccess = "true"))
	float LeaningRate;
	// ----------------------------------
	// --------------Die-----------------
	// ----------------------------------
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Die", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	void UpdateDead();

	// ----------------------------------
	//				Sprint
	// ----------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sprint", meta = (AllowPrivateAccess = "true"))
	bool bSprint = false;
	void UpdateSprint(class ACPP_PlayableCharacter* Character);

	// ----------------------------------
	//				Crouch
	// ----------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching = false;
};
