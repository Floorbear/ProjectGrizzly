// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_ADS.h"
#include "TickTask.h"
#include "..\CPP_PlayableCharacter.h"
#include "Kismet\KismetMathLibrary.h"

UCPP_GA_ADS::UCPP_GA_ADS()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::ADS;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
}

void UCPP_GA_ADS::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	//if (!CanBeCanceled())
	//{
	//	return;
	//}
	//Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	bShouldCancel = true;
	bADS = false;
	UE_LOG(LogTemp, Warning, TEXT("ADS is Cancel"));

}

void UCPP_GA_ADS::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bShouldCancel)
	{
		return;
	}
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	bADS = true;
}

void UCPP_GA_ADS::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bShouldCancel)
	{
		return;
	}
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	bADS = false;
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

	UE_LOG(LogTemp, Warning, TEXT("ADS is On"));
}

void UCPP_GA_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bShouldCancel = false;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("ADS is Off"));
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
		return 20.f;
	}
	return 6.f;
}
