// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Shoot.generated.h"
/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_Shoot : public UGrizzlyAbility
{
	GENERATED_BODY()

public:
	UCPP_GA_Shoot();
	
	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Input")
	bool bPressed = true;


	void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;




	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;

	UFUNCTION()
	void OnEndAbility(FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable)
	bool CanContinueShoot();

	UFUNCTION(BlueprintCallable)
	void Shoot();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_Shoot();
	UFUNCTION(BlueprintCallable)
	float GetRPM() const;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void Tick(float _DeltaTime);

	//--------------------------------------------------------------------------------------------------------
	//												Semi Auto
	//--------------------------------------------------------------------------------------------------------
private:
	bool bCanShootMoreWhenSemiAuto = false;
	// ------------------------
	// --------Recoil----------
	// ------------------------
private:
	
	//UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	//class UCurveVector* WeaponRecoilCurve;
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* MontageTask;
	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* EventReceivedTask;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	class UAbilityTask_WaitDelay* WaitTask;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	float RecoilTime;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	int ShootCount = 0;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	int MaxShootCount = 30;

	UFUNCTION()
	void OnShootWaitFinish();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> VisualRecoil;

public:




	// ------------------------
	// -------- Bullet --------
	// ------------------------
private:
	UFUNCTION( Category = "Bullet")
	void SpawnBullet(const FTransform& CharacterTransform,const FTransform& BulletTransform);


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> BulletDamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> BulletHitParticleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet", meta = (AllowPrivateAccess = "true"))
	float BulletDamage = 40.f;

public:
	UFUNCTION(BlueprintCallable)
	FTransform CalculateBulletSpawnTransform(const FTransform& _CameraPos) const;

	// ------------------------
	// ------- Sync Loading ---
	// ------------------------
private:
	void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// ------------------------
	//			AI
	// ------------------------
	UFUNCTION()
	void OnReceiveStopShoot(FGameplayEventData Payload);

	// ------------------------
	//			Threat
	// ------------------------
	private:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Treat", meta = (AllowPrivateAccess = "true"))
	//float ThreatRadius = 500.f;

	void ApplyThreatToAICharacter(const FTransform& CharacterTransform);

	float GetWeaponThreat();

};
