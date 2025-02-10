// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine\DataTable.h>
#include <Curves\CurveVector.h>
#include "WeaponData.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	No,
	AssaultRilfe			UMETA(DisplayName = "AssaultRilfe"),
	Pistol			UMETA(DisplayName = "Pistol")
};

UENUM(BlueprintType)
enum class EWeaponMode : uint8
{
	Auto			UMETA(DisplayName = "Auto"),
	SemiAuto			UMETA(DisplayName = "SemiAuto")
};

UENUM(BlueprintType)
enum class ECaliber: uint8
{
	No ,
	C76239			UMETA(DisplayName = "7.62x39mm"),
	C45ACP			UMETA(DisplayName = "45ACP")
};

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
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EWeaponMode> WeaponModes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RPM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RandomSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Rounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECaliber Caliber;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoilRecoveryTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Loudness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Threat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThreatRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SocketMode; //총 애니메이션에 로케이션 정보가 있으면 false
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector IdleHandsLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ADSHandsLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator HandsRotation;
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
	TSoftObjectPtr<UAnimSequenceBase> Weapon_Idle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimSequenceBase> Weapon_EmptyIdle;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Weapon_Shoot;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UAnimMontage> Weapon_LastShoot;
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