// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "ProjectGrizzly/Character/CPP_Player.h"
#include "CPP_GA_UseItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_UseItem : public UGrizzlyAbility
{
	GENERATED_BODY()
	UCPP_GA_UseItem();
	void InitVisualAsset(ACPP_Player* Player);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
private:
	UPROPERTY()
	 const ACPP_Item* Item = nullptr;

	TArray<TSubclassOf<UGameplayEffect>> ItemEffects;




	//--------------------------------------------------------------------------------------------------
	//										Draw & UnDraw
	//--------------------------------------------------------------------------------------------------
	UPROPERTY()
	ACPP_WeaponInstance* SourceWeaponInstance = nullptr;
	void DoUndrawWeapon();
	void DoDrawItem();

	UFUNCTION()
	void OnUndrawWeaponCompleted(FGameplayEventData _Data);
	UFUNCTION()
	void OnDrawItemCompleted(FGameplayEventData _Data);
	UFUNCTION()
	void OnDrawItemStarted(FGameplayEventData _Data);
	UFUNCTION()
	void OnDrawWeaponCompleted(FGameplayEventData _Data);
	UFUNCTION()
	void OnDrawWeaponStarted(FGameplayEventData _Data);
	//--------------------------------------------------------------------------------------------------
	//										Montage
	//--------------------------------------------------------------------------------------------------
	UAnimMontage* GetCurrentWeaponUndrawAM() const;
	UAnimMontage* GetCurrentWeaponDrawAM() const;
	UPROPERTY()
	UAnimMontage* AM_Hands = nullptr;
	UPROPERTY()
	UAnimMontage* AM_Item = nullptr;

	//--------------------------------------------------------------------------------------------------
	//										Transform
	//--------------------------------------------------------------------------------------------------
	FRotator PrevRotator;
	FVector	PrevLocation;
};
