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

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Montage")
	UAnimMontage* AM_Hands_Shoot;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Montage")
	UAnimMontage* AM_Weapon_Shoot;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Montage")
	UAnimMontage* AM_Hip_Shoot;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Montage")
	UAnimMontage* AM_ADS_Shoot;

	UFUNCTION()
	void OnEndAbility(FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable)
	bool CanContinueShoot();

	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable)
	float GetRPM();


	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void Tick(float _DeltaTime);

	// ------------------------
	// --------Recoil----------
	// ------------------------
private:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	class UCurveVector* WeaponRecoilCurve;
	class UAbilityTask_PlayMontageAndWait* MontageTask;
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
	float RecoilRecoveryTime = 0.15f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> VisualRecoil;

};
