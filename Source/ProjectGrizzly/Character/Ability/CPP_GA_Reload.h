// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Reload.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_Reload : public UGrizzlyAbility
{
	GENERATED_BODY()
public:
	UCPP_GA_Reload();
private:
	UFUNCTION()
	void OnCompleted();

	bool bIsBindDelegate = false;
	UFUNCTION()
	void BindReloadMontage();
	
protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ActivateAbility();
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Reload")
	class UAnimMontage* TPReloadMontage;

	//ToDo : 추후 Weapon 액터에서 참조하기 때문에 삭제될 변수
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Reload")
	class UAnimMontage* ShouldDelete_HandsReloadMontage;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Reload")
	class UAnimMontage* ShouldDelete_HandsTacReloadMontage;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Reload")
	class UAnimMontage* ShouldDelete_WeaponReloadMontage;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Reload")
	class UAnimMontage* ShouldDelete_WeaponTacReloadMontage;

	//ToDo : Weapon 액터가 구현되면, 거기서 몽타주를 참조할 것
	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetCurrentHandsReloadMontage();
	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetCurrentWeaponReloadMontage();
	//ToDo : Weapon 액터에서 값 참조하기
	UFUNCTION(BlueprintCallable)
	float GetCurrentTPReloadRate();

	
};
