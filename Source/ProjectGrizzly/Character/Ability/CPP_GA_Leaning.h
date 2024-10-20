// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Leaning.generated.h"

/**
 *
 */

 // 플레이어의 LeaningAixs는 단순히 입력 값
 // GAS에서 입력값을 받고 처리함
 // 카메라 보간은 플레이어가 한다. 이유 : 어빌리티는 비활성화되면 업데이트가 중지되기 때문
 // 애니메이션 보간도 같은 이유로 애님인스턴스에서 한다
 
 // ToDo : 토글 기울이기에 대한 처리
UCLASS()
class PROJECTGRIZZLY_API UCPP_GA_Leaning : public UGrizzlyAbility
{
	GENERATED_BODY()

	UCPP_GA_Leaning();

private:
	float LeaningAxis = 0.f;

	UFUNCTION()
	void TimerTick();

	float HandsPitch = 0.f;
	float CurrentHandsPitch = 0.f;


public:
	UFUNCTION(BlueprintCallable)
	float GetLeaningAxis() const
	{
		return LeaningAxis;
	}

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
};
