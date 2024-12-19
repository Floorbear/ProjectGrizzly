// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GrizzlyPS.h"
#include "AbilitySystemComponent.h"
#include "Ability\GrizzlyAttributeSet.h"
#include "CPP_PlayableCharacter.h"
#include "..\Weapon\WeaponComponent.h"
#include "NEt\UnrealNetwork.h"

ACPP_GrizzlyPS::ACPP_GrizzlyPS()
{
	AbilitySystemCompoent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemCompoent->SetIsReplicated(true);

	AbilitySystemCompoent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGrizzlyAttributeSet>(TEXT("AttributeSet"));

	FactionComponent = CreateDefaultSubobject<UFactionComponent>(TEXT("Faction"));
	FactionComponent->SetIsReplicated(true);

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

float ACPP_GrizzlyPS::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float ACPP_GrizzlyPS::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float ACPP_GrizzlyPS::GetSpeed() const
{
	return AttributeSet->GetSpeed();
}

float ACPP_GrizzlyPS::GetStamina() const
{
	if (!IsValid(AttributeSet))
	{
		UE_LOG(LogTemp,Warning,TEXT("ACPP_GrizzlyPS::GetStamina : AttributeSet is not vaild"));
		return 0;
	}
	return AttributeSet->GetStamina();
}

float ACPP_GrizzlyPS::GetMaxStamina() const
{
	if (!IsValid(AttributeSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("ACPP_GrizzlyPS::GetMaxStamina() : AttributeSet is not vaild"));
		return 0;
	}
	return AttributeSet->GetMaxStamina();
}

int ACPP_GrizzlyPS::GetCurrentMagazineRounds() const
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetPawn());
	if (!IsValid(Character))
	{
		return 0;
	}
	return Character->GetWeaponComponent()->GetCurrentMagazineRounds();
}

int ACPP_GrizzlyPS::GetRemainRounds() const
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetPawn());
	if (!IsValid(Character))
	{
		return 0;
	}
	return Character->GetWeaponComponent()->GetRemainRounds();
}

bool ACPP_GrizzlyPS::IsAlive() const
{
	return GetHealth() > 0.f;
}

EFaction ACPP_GrizzlyPS::GetFaction() const
{
	if(FactionComponent == nullptr)
	{
		int a = 0;
	}
	return FactionComponent->GetFaction();
}

void ACPP_GrizzlyPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_GrizzlyPS,bIsDead);
}

void ACPP_GrizzlyPS::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystemCompoent)
	{
		AbilitySystemCompoent->
		GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this,&ACPP_GrizzlyPS::HealthChanged);
		AbilitySystemCompoent->
		GetGameplayAttributeValueChangeDelegate(AttributeSet->GetSpeedAttribute()).AddUObject(this, &ACPP_GrizzlyPS::SpeedChanged);
	}
	
}

void ACPP_GrizzlyPS::HealthChanged(const FOnAttributeChangeData& Data)
{
	if (!IsAlive() && HasAuthority())
	{
		ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(GetPawn());
		Character->Die();
	}

}

void ACPP_GrizzlyPS::SpeedChanged(const struct FOnAttributeChangeData& Data)
{
	if (IsAlive() && HasAuthority())
	{
		ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(GetPawn());
		Character->SetSpeed(GetSpeed());
	}

}

