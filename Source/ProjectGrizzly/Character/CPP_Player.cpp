// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Player.h"
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

//Client
void ACPP_Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	if (!PS)
	{
		return;
	}

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();
	BindASCInput();
	SetSpeed(AttributeSet->GetSpeed());
}	

void ACPP_Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	if (!PS)
	{
		return;
	}

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();

	//(리스폰 할 경우) 이전 이펙트 삭제
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.State.Dead"));
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);

	//스타트 이펙트 적용
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);
		for (auto Effect : StartEffects)
		{
			FGameplayEffectSpecHandle Handle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, Context);
			FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
		}
	}
	SetSpeed(AttributeSet->GetSpeed());


	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || PS->bIsAbilityGiven)
	{
		return;
	}
	PS->bIsAbilityGiven = true;
	//어빌리티 부여
	for (auto ability : PlayerAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(ability, 1, static_cast<int32>(Cast<UGrizzlyAbility>(ability->GetDefaultObject())->InputID), this));
	}
}

void ACPP_Player::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("Confirm"),
			FString("Cancel"), FString("EPGAbilityInputID"), static_cast<int32>(EPGAbilityInputID::Confirm), static_cast<int32>(EPGAbilityInputID::Cancel)));

		ASCInputBound = true;
	}
}

void ACPP_Player::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	BindASCInput();
}

void ACPP_Player::BeginPlay()
{
	Super::BeginPlay();

	if (IsMyComputer())
	{
		GetMesh()->HideBoneByName(TEXT("bip01_spine"), EPhysBodyOp::PBO_None);
		GetTPWeaponComponent()->SetVisibility(false);
	}
	else
	{
		GetHandsMeshComponent()->SetVisibility(false, true);
	}
}
