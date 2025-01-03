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
	// LeftHandIK�� �����ϴ� ��� �� ��Ÿ�ְ� ����Ǹ� LeftHandIK �� ON�մϴ�
	UFUNCTION()
	void AnimNotify_OffLeftHandIKWhenMontage();

	void OnMontageBlendingOutWhenOffIK(UAnimMontage* Montage, bool bInterrupted);

	//ToDo : Struct ȭ �ؾ���, ���� : Linked AnimGraph�� �ѱ� �Ķ���͸� ����ȭ�ϱ� ���ؼ�
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "AnimSequence", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* IdleSequence;
};
