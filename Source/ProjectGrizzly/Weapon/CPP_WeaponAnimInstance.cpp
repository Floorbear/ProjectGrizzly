// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_WeaponAnimInstance.h"
#include "..\Character\CPP_PlayableCharacter.h"

void UCPP_WeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(TryGetPawnOwner());
	if (!IsValid(Player))
	{
		return;
	}

	USkeletalMeshComponent* HandSKComponent = Player->GetHandsMeshComponent();
	FTransform LeftHandTransform = HandSKComponent->GetSocketTransform(TEXT("LeftHand"), RTS_Component);
	LeftHandLocation = LeftHandTransform.GetLocation();
}

void UCPP_WeaponAnimInstance::AnimNotify_OnLeftHandIK()
{
	LeftHandIKAlpha = 1.f;
}

void UCPP_WeaponAnimInstance::AnimNotify_OffLeftHandIK()
{
	LeftHandIKAlpha = 0.f;
}
