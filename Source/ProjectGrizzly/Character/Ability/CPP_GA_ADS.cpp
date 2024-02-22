// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_ADS.h"
#include "TickTask.h"
#include "..\CPP_PlayableCharacter.h"
#include "..\CPP_A_PGCharacter.h"
#include "AbilitySystemComponent.h"
#include "Kismet\KismetMathLibrary.h"
#include "Abilities\Tasks\AbilityTask_WaitInputPress.h"
#include "Abilities\Tasks\AbilityTask_WaitInputRelease.h"


UCPP_GA_ADS::UCPP_GA_ADS()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::ADS;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
}

void UCPP_GA_ADS::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	bShouldCancel = true;
	bADS = false;
}

void UCPP_GA_ADS::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bShouldCancel)
	{
		return;
	}
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	bADS = true;
	if (ADSRelieveHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ADSRelieveHandle);
	}
}

void UCPP_GA_ADS::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bShouldCancel)
	{
		return;
	}
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	bADS = false;
	FGameplayEffectContextHandle Context = CurrentActorInfo->AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GE_ADSRelieve, 1, Context);
	//ADSRelieveHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle); << 이걸 쓰지못하는 이유는 서버 호출도 아니고, PredictionKey 입력도 아니기 때문
	ADSRelieveHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UCPP_GA_ADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	bADS = true;
	UTickTask* Tick = UTickTask::CreateTickTask(this, NAME_None);//->
	Tick->OnTick.AddDynamic(this, &UCPP_GA_ADS::Tick);
	Tick->ReadyForActivation();

	//ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	//UCPP_A_PGCharacter* AnimInstance = Cast<UCPP_A_PGCharacter>(Player->GetMesh()->GetAnimInstance());
	//AnimInstance->bADS = true;

}

void UCPP_GA_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bShouldCancel = false;
	if (ADSRelieveHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ADSRelieveHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCPP_GA_ADS::Tick(float _DeltaTime)
{
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	if (!IsValid(Player))
	{
		return;
	}

	USkeletalMeshComponent* Hands = Player->GetHandsMeshComponent();
	FVector CurrentLocation = Hands->GetRelativeLocation();
	FVector InterpoValue = { 0,0,0 };
	if (bADS)
	{
		InterpoValue = FMath::VInterpTo(CurrentLocation, { 0,0,0 }, _DeltaTime, GetADSSpeed());
		Hands->SetRelativeLocation(InterpoValue);
	}
	else
	{
		InterpoValue = FMath::VInterpTo(CurrentLocation, IdleHandLocation, _DeltaTime, GetADSSpeed());
		Hands->SetRelativeLocation(InterpoValue);

		//줌을 거의 다 풀면 어빌리티 비활성화
		float Distance = FVector::Distance(InterpoValue, IdleHandLocation);
		float MarginOfError = 0.02;
		if (Distance < MarginOfError)
		{
			Hands->SetRelativeLocation(IdleHandLocation);
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
	}
}

float UCPP_GA_ADS::GetADSSpeed()
{
	if (bShouldCancel)
	{
		return 25.f;
	}
	return 6.f;
}



