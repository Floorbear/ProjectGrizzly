// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_Shoot.h"
#include "..\CPP_PlayableCharacter.h"
#include "..\CPP_A_PGCharacter.h"
#include "..\CPP_AIPlayableCharacter.h"
#include "..\CPP_AI_PlayableCharacter_PC.h"
#include "AbilitySystemComponent.h"
#include "Abilities\Tasks\AbilityTask_PlayMontageAndWait.h"
#include "Abilities\Tasks\AbilityTask_WaitGameplayEvent.h"
#include "Abilities\Tasks\AbilityTask_WaitDelay.h"
#include "Kismet\KismetMathLibrary.h"
#include "Kismet\KismetSystemLibrary.h"
#include "TickTask.h"
#include "..\..\Weapon\\CPP_Bullet.h"
#include "..\..\Weapon\\WeaponComponent.h"
#include "NiagaraSystem.h"

#include "Perception/AISense_Hearing.h"
#include "GameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework\CharacterMovementComponent.h"
#include <Curves\CurveVector.h>
#include <Camera\CameraComponent.h>

UCPP_GA_Shoot::UCPP_GA_Shoot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateInputDirectly = true;
	InputID = EPGAbilityInputID::Shoot;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
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

void UCPP_GA_Shoot::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

FTransform UCPP_GA_Shoot::GetBulletSpawnTransform(const FTransform& _CameraPos) const
{
	FTransform BulletSpawnPos = _CameraPos;
	float RandomValue = GetWeaponComponent()->GetRandomSpread();

	ACPP_PGCharacter* Character = GetCharacter();
	//이동할 경우 랜덤 스프레드 값이 커짐
	float AxisWeight = 1.f;
	float ForwardAxis = Character->MoveForwardAxis;
	float RightAxis = Character->MoveRightAxis;
	if (FMath::IsNearlyEqual(ForwardAxis, 0.f) && FMath::IsNearlyEqual(RightAxis, 0.f))
	{
		AxisWeight = 1.f;
	}
	else
	{
		AxisWeight = 1.5f;
	}

	//정조준 하면 랜덤 스프레드 값이 작아짐
	float ADSFactor = 1.f - Cast<ACPP_PlayableCharacter>(GetCharacter())->GetADSFactor();


	RandomValue *= AxisWeight * ADSFactor;
	auto Rotator = BulletSpawnPos.Rotator();
	Rotator.Yaw += UKismetMathLibrary::RandomFloatInRange(-RandomValue, RandomValue);
	Rotator.Pitch += UKismetMathLibrary::RandomFloatInRange(-RandomValue, RandomValue);

	BulletSpawnPos.SetRotation(Rotator.Quaternion());

	//UE_LOG(LogTemp,Warning,TEXT("%f"),RandomValue);
	return BulletSpawnPos;
}

void UCPP_GA_Shoot::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
#if WITH_EDITOR
	static bool bIsLoadedCue = false;
	//어빌리티가 부여될 때 특정 Cue의 동기로딩 진행
	if (!bIsLoadedCue)
	{
		bIsLoadedCue = true;

		if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
		{
			GCM->LoadNotifyForEditorPreview(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.Gun.MuzzleFlash")));
		}
		
	}
#endif // #if WITH_EDITOR
	
}

void UCPP_GA_Shoot::OnReceiveStopShoot(FGameplayEventData Payload)
{
	bPressed = false;
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

	//Remote로 Shoot을 멈출 수 있게 콜백 등록
	auto StopShootListen = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(("Event.Control.Gunplay.StopShoot")));
	StopShootListen->EventReceived.AddDynamic(this, &UCPP_GA_Shoot::OnReceiveStopShoot);
	StopShootListen->ReadyForActivation();

	if (!GetWeaponComponent()->IsMagazineEmpty())
	{
		UTickTask* TickTask = UTickTask::CreateTickTask(this, NAME_None);
		TickTask->OnTick.AddDynamic(this, &UCPP_GA_Shoot::Tick);
		TickTask->ReadyForActivation();
		Shoot();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

}

void UCPP_GA_Shoot::OnEndAbility(FGameplayEventData Payload)
{
	if (CanContinueShoot())
	{		
		Shoot();
	}
	else
	{
		//Recoil 회복이 끝나면 어빌리티를 종료하겠다

		float RecoilRecoveryTime = GetCharacter()->GetWeaponComponent()->GetRecoilRecoveryTime();
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

	if (GetWeaponComponent()->IsMagazineEmpty())
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

	//탄창에서 총알 감소
	GetWeaponComponent()->SetCurrentMagazineRounds(GetWeaponComponent()->GetCurrentMagazineRounds()-1);

	//RPM 으로 몽타주 Rate 계산
	// 기준 : 1.0 Rate = 400 RPM = 0.15초당 1발
	// Ex ) 800 RPM = 0.075초당 1발 = 2.0Rate
	float RPM = GetRPM();
	float AnimationRate = RPM / 400.0f;

	// 몽타주 재생
	Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("Shoot")), AnimationRate);
	Player->GetFPWeaponMeshComponent()->GetAnimInstance()->Montage_Play(GetWeaponComponent()->Get_FP_Weapon_Montage(TEXT("Shoot")), AnimationRate);
	bool bADS = Player->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	bool bIsCrouching = GetCharacter()->GetCharacterMovement()->IsCrouching();
	if (IsValid(MontageTask))
	{
		MontageTask->EndTask();
	}
	if (IsValid(EventReceivedTask))
	{
		EventReceivedTask->EndTask();
	}


	if (bIsCrouching)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GetWeaponComponent()->Get_TP_Montage(TEXT("Crouch_Shoot")), AnimationRate, NAME_None, false);
	}
	else
	{
		if (bADS)
		{
			MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GetWeaponComponent()->Get_TP_Montage(TEXT("ADSShoot")), AnimationRate, NAME_None, false);
		}
		else
		{
			MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GetWeaponComponent()->Get_TP_Montage(TEXT("HipShoot")), AnimationRate, NAME_None, false);
		}
	}
	
	//노이즈  
	float Loudness = GetWeaponComponent()->GetLoudness();
	UAISense_Hearing::ReportNoiseEvent(GetWorld(),GetAvatarActorFromActorInfo()->GetActorLocation(), Loudness,GetAvatarActorFromActorInfo());

	//카메라 흔들림
	if (!IsAIControl())
	{
		if (CurrentActorInfo->PlayerController != NULL)
		{
			CurrentActorInfo->PlayerController->PlayerCameraManager->StartCameraShake(VisualRecoil);
		}
	}
	MontageTask->ReadyForActivation();

	//머즐 플레쉬
	{
		TSubclassOf<UGameplayEffect> GE_MuzzleFlashEffect = GetWeaponComponent()->GetMuzzleFlashEffect();
		FGameplayEffectContextHandle Context = CurrentActorInfo->AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GE_MuzzleFlashEffect, 1, Context);
		CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	BP_Shoot();
	// 총알 생성
	//if (Player->IsMyComputer() == true)
	if(HasAuthority(&CurrentActivationInfo))
	{
		FTransform CharacterTransform = Player->GetCamera()->GetComponentTransform();
		CharacterTransform.SetRotation(Player->GetController()->GetControlRotation().Quaternion());
		//SpawnBullet(BulletSpawnPos);
		SpawnBullet(CharacterTransform,GetBulletSpawnTransform(CharacterTransform));
	}
	// 다음 사격에 대한 콜백 등록
	EventReceivedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(("Event.Montage.Gunplay.ShootEnd")));
	EventReceivedTask->EventReceived.AddDynamic(this, &UCPP_GA_Shoot::OnEndAbility);
	EventReceivedTask->ReadyForActivation();
}

float UCPP_GA_Shoot::GetRPM() const
{
	float RPM = GetCharacter()->GetWeaponComponent()->GetRPM();
	return RPM;
}

void UCPP_GA_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//현재 진행중인 테스크를 전부 종료
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
	bPressed = true;
	RecoilTime = 0;

	ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);
	Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_StopWithBlendOut(0.1f, GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("Shoot")));
	Player->GetFPWeaponMeshComponent()->GetAnimInstance()->Montage_StopWithBlendOut(0.1f, GetWeaponComponent()->Get_FP_Weapon_Montage(TEXT("Shoot")));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCPP_GA_Shoot::Tick(float _DeltaTime)
{
	//에임 회복 상태에서 사격하면 사격 어빌리티 비활성화 하는 명령을 중지하고 사격을 계속 진행
	if (IsValid(WaitTask))
	{
		if (bPressed && !GetWeaponComponent()->IsMagazineEmpty())
		{
			WaitTask->EndTask();
			WaitTask = NULL;
			Shoot();
		}
	}

	float Rate = GetRPM() / 400.f;
	RecoilTime = RecoilTime + _DeltaTime * Rate;

	//반동
	//반동 처리를 여기서 하는 이유 : 반동 그래프는 '연속적'이므로
	auto WeaponRecoilCurve = GetWeaponComponent()->GetWeaponRecoilCurve();
	float Pitch = WeaponRecoilCurve->GetVectorValue(RecoilTime).X;
	float Yaw = WeaponRecoilCurve->GetVectorValue(static_cast<float>(ShootCount - 1) / static_cast<float>(MaxShootCount)).Y; // Yaw 그래프는 0~1 사이의 값, 따라서 mapping 필요
	float PitchWeight = WeaponRecoilCurve->GetVectorValue(static_cast<float>(ShootCount - 1) / static_cast<float>(MaxShootCount)).Z;

	if (Pitch > 0.f)
	{
		//PitchWeight = UKismetMathLibrary::RandomFloatInRange(1.0, PitchWeight);
		Pitch *= PitchWeight;
	}
	Pitch *= -1.f;
	if (!IsAIControl())
	{
		
		CurrentActorInfo->PlayerController->AddPitchInput(Pitch);
		CurrentActorInfo->PlayerController->AddYawInput(Yaw);
	}

}

void UCPP_GA_Shoot::OnShootWaitFinish()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}



void UCPP_GA_Shoot::SpawnBullet(const FTransform& CharacterTransform, const FTransform& BulletTransform)
{
	ACPP_PGCharacter* Character = GetCharacter();
	ACPP_Bullet* Bullet = GetWorld()->SpawnActorDeferred<ACPP_Bullet>
	(BulletClass, BulletTransform, CurrentActorInfo->OwnerActor.Get(), Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	FGameplayEffectSpecHandle DamageEffectHandle = MakeOutgoingGameplayEffectSpec(BulletDamageEffect,GetAbilityLevel());
	FGameplayEffectSpecHandle HitParticleEffectHandle = MakeOutgoingGameplayEffectSpec(BulletHitParticleEffect,GetAbilityLevel());
	Bullet->DamageEffectHandle = DamageEffectHandle;
	Bullet->HitParticleEffectHandle = HitParticleEffectHandle;
	DamageEffectHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), BulletDamage);
	Bullet->FinishSpawning(BulletTransform);

	ApplyThreatToAICharacter(CharacterTransform);
}

void UCPP_GA_Shoot::ApplyThreatToAICharacter(const FTransform& CharacterTransform)
{
	//Threat 시스템
	FVector EndPos = CharacterTransform.GetLocation() + CharacterTransform.GetRotation().Vector() * 100 * 300;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	TArray<FHitResult> HitResults;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(CurrentActorInfo->AvatarActor.Get());
	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), CharacterTransform.GetLocation(), EndPos, ThreatRadius, ObjectTypes, false, IgnoreActors,
		EDrawDebugTrace::None, HitResults, true);

	//트레이스에 충돌된 AI캐릭터의 위협도 증가
	for (FHitResult HitResult : HitResults)
	{
		ACPP_AIPlayableCharacter* Character = Cast<ACPP_AIPlayableCharacter>(HitResult.GetActor());
		if (Character == NULL)
		{
			break;
		}
		ACPP_AI_PlayableCharacter_PC* Controller = Cast<ACPP_AI_PlayableCharacter_PC>(Character->GetController());
		Controller->SetBulletThreat(Controller->GetBulletThreat()+ GetWeaponThreat());
	}
}

float UCPP_GA_Shoot::GetWeaponThreat()
{
	return 0.2f;
}
