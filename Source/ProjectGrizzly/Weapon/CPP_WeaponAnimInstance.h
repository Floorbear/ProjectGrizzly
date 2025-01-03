// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CPP_WeaponAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_WeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
	void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category = "IK", meta = (AllowPrivateAccess = "true"))
	FVector LeftHandLocation;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category = "IK", meta = (AllowPrivateAccess = "true"))
	FVector RightHandLocation;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator RightHandRotator;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "IK", meta = (AllowPrivateAccess = "true"))
	float LeftHandIKAlpha = 1.0f;

	UFUNCTION()
	void AnimNotify_OnLeftHandIK();
	UFUNCTION()
	void AnimNotify_OffLeftHandIK();
	// LeftHandIK를 해제하는 대신 이 몽타주가 종료되면 LeftHandIK 를 ON합니다
	UFUNCTION()
	void AnimNotify_OffLeftHandIKWhenMontage();

	void OnMontageBlendingOutWhenOffIK(UAnimMontage* Montage, bool bInterrupted);

	//ToDo : Struct 화 해야함, 이유 : Linked AnimGraph에 넘길 파라미터를 간소화하기 위해서
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* IdleSequence;
};
