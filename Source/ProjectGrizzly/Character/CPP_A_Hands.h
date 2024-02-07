// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CPP_PlayableCharacter.h"
#include "CPP_A_Hands.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_A_Hands : public UAnimInstance
{
	GENERATED_BODY()
public:
	UCPP_A_Hands();


	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	TWeakObjectPtr<class ACPP_PlayableCharacter> Player;
public:
	class ACPP_PlayableCharacter* GetPlayer();

	// ----- Movement -----
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"));
	float MoveForwardAxis = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"));
	float MoveRightAxis = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandSway", meta = (AllowPrivateAccess = "true"));
	float MouseXDelta = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandSway", meta = (AllowPrivateAccess = "true"));
	float MouseYDelta = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandSway", meta = (AllowPrivateAccess = "true"));
	float HandSwayXFactorDuringAiming= 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandSway", meta = (AllowPrivateAccess = "true"));
	float HandSwayYFactorDuringAiming= 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandSway", meta = (AllowPrivateAccess = "true"));
	FRotator HandSwayRotator;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (AllowPrivateAccess = "true"));
	UAnimSequenceBase* IdleSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (AllowPrivateAccess = "true"));
	UAnimSequenceBase* WalkSequence;
};
