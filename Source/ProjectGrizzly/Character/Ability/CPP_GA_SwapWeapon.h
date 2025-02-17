// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "..\..\Weapon/WeaponData.h"
#include "CPP_GA_SwapWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_SwapWeapon : public UGrizzlyAbility
{

	GENERATED_BODY()

public:
	UCPP_GA_SwapWeapon();
	void DoUndraw();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	EWeaponSlot TargetSlot;
	UPROPERTY()
	class ACPP_WeaponInstance* TargetWeaponInstance = nullptr;
	UPROPERTY()
	class ACPP_WeaponInstance* SourceWeaponInstance = nullptr;

private:
	bool IsUnarmed(class ACPP_WeaponInstance* _WeaponInstance) const;
	UAnimMontage* GetCurrentWeaponUndrawAM() const;
	UAnimMontage* GetCurrentWeaponDrawAM() const;
	void DoDraw();
	bool StartDraw();


	UFUNCTION()
	void OnUndrawCompleted(FGameplayEventData _Data);
	UFUNCTION()
	void OnDrawCompleted(FGameplayEventData _Data);
	UFUNCTION()
	void OnDrawStarted(FGameplayEventData _Data);
};

