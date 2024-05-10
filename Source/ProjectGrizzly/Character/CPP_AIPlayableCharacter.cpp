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
#include "..\Weapon\WeaponComponent.h"
#include "Net\UnrealNetwork.h"

#include "ProjectGrizzly/ProjectGrizzly.h"
//ToDo : AI쪽으로 가야할 헤더
#include "AIModule\Classes\AIController.h"
ACPP_AIPlayableCharacter::ACPP_AIPlayableCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UGrizzlyAttributeSet>(TEXT("AttributeSet"));
	checkf(IsValid(AttributeSet.Get()), TEXT("Not Created"))
	bIsAI = true;
}

void ACPP_AIPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
//	UE_LOG(LogTemp, Warning, TEXT("%f << Current HP"), AttributeSet->GetHealth());
}

void ACPP_AIPlayableCharacter::BeginPlay()
{

	Super::BeginPlay();
	//(리스폰 할 경우) 이전 이펙트 삭제
	if (HasAuthority())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		FGameplayTagContainer Container;
		Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.State.Dead"));
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);
		//스타트 이펙트 적용
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);
		for (auto Effect : StartEffects)
		{
			FGameplayEffectSpecHandle Handle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, Context);
			FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
		}


		//어빌리티 부여
		for (auto ability : PlayerAbilities)
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(ability, 1, static_cast<int32>(Cast<UGrizzlyAbility>(ability->GetDefaultObject())->InputID), this));
		}

		if (IsValid(AbilitySystemComponent.Get()))
		{
			AbilitySystemComponent->
				GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ACPP_AIPlayableCharacter::HealthChanged);
		}
	}
	

	//1인칭 메쉬 숨기기 
	GetHandsMeshComponent()->SetVisibility(false, true);
}

void ACPP_AIPlayableCharacter::HealthChanged(const struct FOnAttributeChangeData& Data)
{
	if (!IsAlive() && HasAuthority())
	{
		Die();
	}
}

bool ACPP_AIPlayableCharacter::IsAlive() const
{
	return GetHealth() > 0.f;
}

float ACPP_AIPlayableCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}


