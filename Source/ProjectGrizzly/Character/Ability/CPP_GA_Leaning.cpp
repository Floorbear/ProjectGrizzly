// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Leaning.h"
#include "..\CPP_PlayableCharacter.h"
#include "..\CPP_Player.h"

#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UCPP_GA_Leaning::UCPP_GA_Leaning()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//bReplicateInputDirectly 는 InputReleased , InputPressed 함수를 사용할 때 true로 해야 함
	bReplicateInputDirectly = true;
	InputID = EPGAbilityInputID::Leaning;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Leaning")));

	//달리는 도중에 기울이면 달리기 취소
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Sprint")));

}

void UCPP_GA_Leaning::TimerTick()
{
	ACPP_PlayableCharacter* Character = GetCharacter();

	//플레이어 일경우 LeaningAxis값이 변화중일 때 HandSway 기능을 추가
	ACPP_Player* Player = Cast<ACPP_Player>(Character);
	float LeaningDelta = LeaningAxis - Character->GetRawLeaningAxis();
	if (Player)
	{
		if (FMath::IsNearlyZero(LeaningDelta, 0.01f))
		{
			HandsPitch = 0.f;
		}
		else if (LeaningDelta > 0)
		{
			Player->HandSwayXDelta = -1.f;
			HandsPitch = 5.f;
			Player->GetHandsMeshComponent()->AddRelativeLocation({0.f,CurrentHandsPitch,0.f});
		}
		else if (LeaningDelta < 0)
		{
			Player->HandSwayXDelta = 1.f;
			HandsPitch = -5.f;
			Player->GetHandsMeshComponent()->AddRelativeLocation({ 0.f,CurrentHandsPitch,0.f});
		}

	}
	LeaningAxis = Character->GetRawLeaningAxis();

	Character->SetLeaningAxis(LeaningAxis);

	CurrentHandsPitch = FMath::Lerp(CurrentHandsPitch,HandsPitch,0.05f * 3.f);
	UAbilityTask_WaitDelay* Delay = UAbilityTask_WaitDelay::WaitDelay(this, 0.05f);
	Delay->OnFinish.AddDynamic(this, &UCPP_GA_Leaning::TimerTick);
	Delay->ReadyForActivation();
}





void UCPP_GA_Leaning::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	Super::ActivateAbility(Handle,ActorInfo,ActivationInfo, TriggerEventData);
	ACPP_PlayableCharacter* Character = GetCharacter();
	// Leaning 변수 설정
	LeaningAxis = Character->GetRawLeaningAxis();

	// Raw Leaning 값 업데이트를 확인하는 테스트
	UAbilityTask_WaitDelay* Delay = UAbilityTask_WaitDelay::WaitDelay(this, 0.05f);
	Delay->OnFinish.AddDynamic(this, &UCPP_GA_Leaning::TimerTick);
	Delay->ReadyForActivation();
}

void UCPP_GA_Leaning::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	LeaningAxis = 0.f;
	CurrentHandsPitch = 0.f;
	HandsPitch = 0.f;
	//HandSway 
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	float LeaningDelta = LeaningAxis - GetCharacter()->GetRawLeaningAxis();
	if (Player)
	{
		if (FMath::IsNearlyZero(LeaningDelta, 0.01f))
		{

		}
		else if (LeaningDelta > 0)
		{
			Player->HandSwayXDelta = 1.f;
		}
		else if (LeaningDelta < 0)
		{
			Player->HandSwayXDelta = -1.f;
		}

	}
	//Leaning 변수 초기화
	GetCharacter()->SetLeaningAxis(LeaningAxis);

}

void UCPP_GA_Leaning::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_EndAbility();
}
