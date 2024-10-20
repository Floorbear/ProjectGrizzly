// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Leaning.h"
#include "..\CPP_PlayableCharacter.h"
#include "..\CPP_Player.h"

#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UCPP_GA_Leaning::UCPP_GA_Leaning()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//bReplicateInputDirectly �� InputReleased , InputPressed �Լ��� ����� �� true�� �ؾ� ��
	bReplicateInputDirectly = true;
	InputID = EPGAbilityInputID::Leaning;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Leaning")));

	//�޸��� ���߿� ����̸� �޸��� ���
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Sprint")));

}

void UCPP_GA_Leaning::TimerTick()
{
	ACPP_PlayableCharacter* Character = GetCharacter();

	//�÷��̾� �ϰ�� LeaningAxis���� ��ȭ���� �� HandSway ����� �߰�
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
	// Leaning ���� ����
	LeaningAxis = Character->GetRawLeaningAxis();

	// Raw Leaning �� ������Ʈ�� Ȯ���ϴ� �׽�Ʈ
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
	//Leaning ���� �ʱ�ȭ
	GetCharacter()->SetLeaningAxis(LeaningAxis);

}

void UCPP_GA_Leaning::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_EndAbility();
}
