// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine\DataTable.h>
#include <Curves\CurveVector.h>
#include "WeaponData.generated.h"


/**
 * 
 */

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int ItemClassID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RPM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RandomSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Rounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoilRecoveryTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Loudness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Threat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThreatRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UCurveVector> WeaponRecoilCurve;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<class UGameplayEffect> MuzzleFlashEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> WeaponSkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> WeaponAnimClass;

};

USTRUCT(BlueprintType)
struct FWeaponAnim : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Weapon_Shoot;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Hands_Shoot;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Weapon_Reload;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Hands_Reload;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Weapon_TacReload;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Hands_TacReload;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimSequenceBase> Hands_WalkForward;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimSequenceBase> Hands_WalkBackward;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimSequenceBase> Hands_Idle;
		UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimSequenceBase> Hands_Sprint;
};