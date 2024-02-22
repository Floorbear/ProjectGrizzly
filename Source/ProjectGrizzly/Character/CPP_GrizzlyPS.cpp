// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GrizzlyPS.h"
#include "AbilitySystemComponent.h"
#include "Ability\GrizzlyAttributeSet.h"

ACPP_GrizzlyPS::ACPP_GrizzlyPS()
{
	AbilitySystemCompoent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemCompoent->SetIsReplicated(true);

	AbilitySystemCompoent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGrizzlyAttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* ACPP_GrizzlyPS::GetAbilitySystemComponent() const
{
	return AbilitySystemCompoent;
}

UGrizzlyAttributeSet* ACPP_GrizzlyPS::GetAttributeSet() const
{
	return AttributeSet;
}
