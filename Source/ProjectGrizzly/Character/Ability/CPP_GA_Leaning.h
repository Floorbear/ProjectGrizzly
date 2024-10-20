// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrizzlyAbility.h"
#include "CPP_GA_Leaning.generated.h"

/**
 *
 */

 // �÷��̾��� LeaningAixs�� �ܼ��� �Է� ��
 // GAS���� �Է°��� �ް� ó����
 // ī�޶� ������ �÷��̾ �Ѵ�. ���� : �����Ƽ�� ��Ȱ��ȭ�Ǹ� ������Ʈ�� �����Ǳ� ����
 // �ִϸ��̼� ������ ���� ������ �ִ��ν��Ͻ����� �Ѵ�
 
 // ToDo : ��� ����̱⿡ ���� ó��
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
