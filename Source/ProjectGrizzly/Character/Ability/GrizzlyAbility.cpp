// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyAbility.h"
#include "Abilities\Tasks\AbilityTask_WaitGameplayEvent.h"
#include "..\..\Weapon\\WeaponComponent.h"
#include "..\CPP_PlayableCharacter.h"
#include "AIModule\Classes\AIController.h"
#include "..\CPP_A_PGCharacter.h"


class ACPP_PlayableCharacter* UGrizzlyAbility::GetCharacter() const
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetCurrentActorInfo()->AvatarActor);
	return Character;
}

class UWeaponComponent* UGrizzlyAbility::GetWeaponComponent() const
{
	return GetCharacter()->GetWeaponComponent();
}

bool UGrizzlyAbility::IsAIControl() const
{
	if (CurrentActorInfo->PlayerController == NULL)
	{
		return true;
	}
	return false;
}
