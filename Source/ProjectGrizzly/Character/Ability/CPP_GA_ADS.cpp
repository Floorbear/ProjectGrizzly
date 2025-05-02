// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_ADS.h"
#include "TickTask.h"
#include "..\CPP_PlayableCharacter.h"
#include "..\CPP_A_PGCharacter.h"
#include "AbilitySystemComponent.h"
#include "Kismet\KismetMathLibrary.h"
#include "Abilities\Tasks\AbilityTask_WaitInputPress.h"
#include "Abilities\Tasks\AbilityTask_WaitInputRelease.h"
#include "Abilities/\Tasks/AbilityTask_WaitGameplayEvent.h"


UCPP_GA_ADS::UCPP_GA_ADS()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputID = EPGAbilityInputID::ADS;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));

	static auto ADS_In_Object = ConstructorHelpers::FObjectFinder<USoundBase>(TEXT("/Game/ProjectGrizzly/Gun/Sound/ADS_In.ADS_In"));
	if(ADS_In_Object.Succeeded())
	{
		ADS_In_Sound = ADS_In_Object.Object;
	}

	static auto ADS_Out_Object = ConstructorHelpers::FObjectFinder<USoundBase>(TEXT("/Game/ProjectGrizzly/Gun/Sound/ADS_Out.ADS_Out"));
	if(ADS_Out_Object.Succeeded())
	{
		ADS_Out_Sound = ADS_Out_Object.Object;
	}
}

void UCPP_GA_ADS::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	bShouldCancel = true;
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	if(Player)
	{
		Player->SetADS(false);
	}

	//Super::K2_CancelAbility();
}

void UCPP_GA_ADS::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bShouldCancel)
	{
		return;
	}
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	Player->SetADS(true);

	ApplyADSEffect();

	//ADS 소리 재생
	Player->PlaySoundAtComponent(Player->GetWeaponAudioComponent(),ADS_In_Sound);

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
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	Player->SetADS(false);

	ApplyADSRelieveEffect();

	//ADS 소리 재생
	Player->PlaySoundAtComponent(Player->GetWeaponAudioComponent(),ADS_Out_Sound);
	
	if (ADSHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ADSHandle);
	}

}

void UCPP_GA_ADS::ApplyADSRelieveEffect()
{
	FGameplayEffectContextHandle Context = CurrentActorInfo->AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GE_ADSRelieve, 1, Context);
	ADSRelieveHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UCPP_GA_ADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	Player->SetADS(true);
	
	ApplyADSEffect();

	//ADS 소리 재생
	Player->PlaySoundAtComponent(Player->GetWeaponAudioComponent(),ADS_In_Sound);

	UTickTask* Tick = UTickTask::CreateTickTask(this, NAME_None);//->
	Tick->OnTick.AddDynamic(this, &UCPP_GA_ADS::Tick);
	Tick->ReadyForActivation();
	

	//ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	//UCPP_A_PGCharacter* AnimInstance = Cast<UCPP_A_PGCharacter>(Player->GetMesh()->GetAnimInstance());
	//AnimInstance->bADS = true;
	// 
	//AI Control Task 생성
	UAbilityTask_WaitGameplayEvent* AIADSRelieveTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADSRelieve")));
	AIADSRelieveTask->EventReceived.AddDynamic(this, &UCPP_GA_ADS::OnReceivedADSRelieveEvent);
	AIADSRelieveTask->ReadyForActivation();
	

}

void UCPP_GA_ADS::ApplyADSEffect()
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GE_ADS);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.MovementSpeed")), GetADSMovementSpeed());
	ADSHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UCPP_GA_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bShouldCancel = false;
	if (ADSRelieveHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ADSRelieveHandle);
	}
	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	Player->SetADS(false);
	//이펙트 제거
	if (ADSRelieveHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ADSRelieveHandle);
	}
	if (ADSHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ADSHandle);
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
	
	// 어빌리티의 종료 체크는 오토노머스에서 하겠다
	bool IsLocallyControlled = Player->IsLocallyControlled();
	if (IsLocallyControlled)
	{
		if (Player->GetADSFactor() < 0.02 && !Player->GetADS())
		{
			K2_EndAbility();
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

void UCPP_GA_ADS::OnReceivedADSRelieveEvent(FGameplayEventData _Payload)
{
	K2_EndAbility();
}



