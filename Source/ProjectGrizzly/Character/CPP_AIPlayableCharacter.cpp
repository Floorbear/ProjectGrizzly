// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_AIPlayableCharacter.h"
#include <GameFramework\SpringArmComponent.h>
#include <Camera\CameraComponent.h>
#include "CPP_GrizzlyPS.h"
#include "Ability\GrizzlyAbility.h"
#include "AbilitySystemComponent.h"
#include "GrizzlyPC.h"
#include <Kismet/KismetMathLibrary.h>
#include <Components/CapsuleComponent.h>
#include "CPP_AI_PlayableCharacter_PC.h"
#include "..\Weapon\WeaponComponent.h"
#include "Net\UnrealNetwork.h"

#include "ProjectGrizzly/ProjectGrizzly.h"
//ToDo : AI������ ������ ���
#include "AIModule\Classes\AIController.h"
ACPP_AIPlayableCharacter::ACPP_AIPlayableCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UGrizzlyAttributeSet>(TEXT("AttributeSet"));
	checkf(IsValid(AttributeSet.Get()), TEXT("Not Created"))
		bIsAI = true;

	FactionComponent = CreateDefaultSubobject<UFactionComponent>(TEXT("Faction"));
	FactionComponent->SetIsReplicated(true);
}

void ACPP_AIPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//	UE_LOG(LogTemp, Warning, TEXT("%f << Current HP"), AttributeSet->GetHealth());
}

EFaction ACPP_AIPlayableCharacter::GetFaction() const
{
	return FactionComponent->GetFaction();
}

float ACPP_AIPlayableCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
	GetAIController()->SetBulletThreat(GetAIController()->GetBulletThreat() + Damage / 100.f); // ������ 40�̸� Threat �� 0.4
	return ActualDamage;
	
}



void ACPP_AIPlayableCharacter::BeginPlay()
{

	Super::BeginPlay();
	//(������ �� ���) ���� ����Ʈ ����
	if (HasAuthority())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		FGameplayTagContainer Container;
		Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.State.Dead"));
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);
		//��ŸƮ ����Ʈ ����
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);
		for (auto Effect : StartEffects)
		{
			FGameplayEffectSpecHandle Handle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, Context);
			FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
		}


		//�����Ƽ �ο�
		for (auto ability : PlayerAbilities)
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(ability, 1, static_cast<int32>(Cast<UGrizzlyAbility>(ability->GetDefaultObject())->InputID), this));
		}

		if (IsValid(AbilitySystemComponent.Get()))
		{
			AbilitySystemComponent->
				GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ACPP_AIPlayableCharacter::HealthChanged);
			AbilitySystemComponent->
				GetGameplayAttributeValueChangeDelegate(AttributeSet->GetSpeedAttribute()).AddUObject(this, &ACPP_AIPlayableCharacter::SpeedChanged);
		}
	}


	//1��Ī �޽� ����� 
	GetHandsMeshComponent()->SetVisibility(false, true);
}

void ACPP_AIPlayableCharacter::HealthChanged(const struct FOnAttributeChangeData& Data)
{
	if (!IsAlive() && HasAuthority())
	{
		Die();
	}
}

void ACPP_AIPlayableCharacter::SpeedChanged(const struct FOnAttributeChangeData& Data)
{
	if (IsAlive() && HasAuthority())
	{
		this->SetSpeed(Data.NewValue);
	}

}

bool ACPP_AIPlayableCharacter::IsAlive() const
{
	return GetHealth() > 0.f;
}

bool ACPP_AIPlayableCharacter::IsDead() const
{
	//���� ������ �������ڸ��� ĳ������ ü���� 0�̵Ǽ� �״� ����� ����
	if (GetHealth() > 0)
	{
		return false;
	}
	return true;
}

float ACPP_AIPlayableCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}

ACPP_AI_PlayableCharacter_PC* ACPP_AIPlayableCharacter::GetAIController() const
{
	return Cast<ACPP_AI_PlayableCharacter_PC>(GetController());
}


