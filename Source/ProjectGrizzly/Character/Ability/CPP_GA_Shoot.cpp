// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Shoot.h"
#include "..\CPP_PlayableCharacter.h"
#include "..\CPP_A_PGCharacter.h"
#include "AbilitySystemComponent.h"
#include "Abilities\Tasks\AbilityTask_PlayMontageAndWait.h"
#include "Abilities\Tasks\AbilityTask_WaitGameplayEvent.h"
#include "Abilities\Tasks\AbilityTask_WaitDelay.h"
#include "Kismet\KismetMathLibrary.h"
#include "TickTask.h"
#include <Curves\CurveVector.h>

UCPP_GA_Shoot::UCPP_GA_Shoot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateInputDirectly = true;
	InputID = EPGAbilityInputID::Shoot;
}


void UCPP_GA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	bPressed = false;
}

void UCPP_GA_Shoot::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	bPressed = true;
}

void UCPP_GA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	RecoilTime = 0;
	ShootCount = 0;
	bPressed = true;

	UTickTask* TickTask = UTickTask::CreateTickTask(this, NAME_None);
	TickTask->OnTick.AddDynamic(this,&UCPP_GA_Shoot::Tick);
	TickTask->ReadyForActivation();
	Shoot();
}

void UCPP_GA_Shoot::OnEndAbility(FGameplayEventData Payload)
{
	if (CanContinueShoot())
	{		
		Shoot();
	}
	else
	{
		//Recoil ȸ���� ������ �����Ƽ�� �����ϰڴ�
		WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, RecoilRecoveryTime);
		WaitTask->OnFinish.AddDynamic(this, &UCPP_GA_Shoot::OnShootWaitFinish);
		WaitTask->ReadyForActivation();
	}
}

bool UCPP_GA_Shoot::CanContinueShoot()
{
	if (!bPressed)
	{
		return false;
	}

	return true;
}

void UCPP_GA_Shoot::Shoot()
{
	RecoilTime = 0;
	ShootCount++;

	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);

	//RPM ���� ��Ÿ�� Rate ���
	// ���� : 1.0 Rate = 400 RPM = 0.15�ʴ� 1��
	// Ex ) 800 RPM = 0.075�ʴ� 1�� = 2.0Rate
	float RPM = GetRPM();
	float AnimationRate = RPM / 400.0f;

	// ��Ÿ�� ���
	Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(AM_Hands_Shoot, AnimationRate);
	Player->GetFPWeaponMeshComponent()->GetAnimInstance()->Montage_Play(AM_Weapon_Shoot, AnimationRate);
	bool bADS = Player->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	if (IsValid(MontageTask))
	{
		MontageTask->EndTask();
	}
	if (IsValid(EventReceivedTask))
	{
		EventReceivedTask->EndTask();
	}
	if (bADS)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_ADS_Shoot, AnimationRate, NAME_None, false);
	}
	else
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_Hip_Shoot, AnimationRate, NAME_None, false);
	}

	//ī�޶� ��鸲
	CurrentActorInfo->PlayerController->PlayerCameraManager->StartCameraShake(VisualRecoil);
	MontageTask->ReadyForActivation();



	// ����� ���� �� �ݹ� ���
	EventReceivedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(("Event.Montage.EndAbility")));
	EventReceivedTask->EventReceived.AddDynamic(this, &UCPP_GA_Shoot::OnEndAbility);
	EventReceivedTask->ReadyForActivation();
}

float UCPP_GA_Shoot::GetRPM()
{
	return 650.0f;
}

void UCPP_GA_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//���� �������� �׽�ũ�� ���� ����
	if (IsValid(MontageTask))
	{
		MontageTask->EndTask();
	}
	if (IsValid(EventReceivedTask))
	{
		EventReceivedTask->EndTask();
	}
	MontageTask = NULL;
	EventReceivedTask = NULL;
	WaitTask = NULL;

	//������� �ʱ�ȭ
	RecoilTime = 0;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCPP_GA_Shoot::Tick(float _DeltaTime)
{
	//���� ȸ�� ���¿��� ����ϸ� ��� �����Ƽ ��Ȱ��ȭ �ϴ� ����� �����ϰ� ����� ��� ����
	if (IsValid(WaitTask))
	{
		if (bPressed)
		{
			WaitTask->EndTask();
			WaitTask = NULL;
			Shoot();
		}
	}

	float Rate = GetRPM() / 400.f;
	RecoilTime = RecoilTime + _DeltaTime * Rate;

	//�ݵ�
	//�ݵ� ó���� ���⼭ �ϴ� ���� : �ݵ� �׷����� '������'�̹Ƿ�
	float Pitch = WeaponRecoilCurve->GetVectorValue(RecoilTime).X;
	float Yaw = WeaponRecoilCurve->GetVectorValue(static_cast<float>(ShootCount - 1) / static_cast<float>(MaxShootCount)).Y; // Yaw �׷����� 0~1 ������ ��, ���� mapping �ʿ�
	float PitchWeight = WeaponRecoilCurve->GetVectorValue(static_cast<float>(ShootCount - 1) / static_cast<float>(MaxShootCount)).Z;

	if (Pitch > 0.f)
	{
		//PitchWeight = UKismetMathLibrary::RandomFloatInRange(1.0, PitchWeight);
		Pitch *= PitchWeight;
	}
	Pitch *= -1.f;
	CurrentActorInfo->PlayerController->AddPitchInput(Pitch);
	CurrentActorInfo->PlayerController->AddYawInput(Yaw);
}

void UCPP_GA_Shoot::OnShootWaitFinish()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
