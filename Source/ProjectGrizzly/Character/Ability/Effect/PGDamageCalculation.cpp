// Fill out your copyright notice in the Description page of Project Settings.


#include "PGDamageCalculation.h"
#include "..\GrizzlyAttributeSet.h"

UPGDamageCalculation::UPGDamageCalculation()
{
	//Relevent
}

void UPGDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 계산된 Damage는 Target에 적용됨

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	FHitResult HitResult = *Spec.GetContext().GetHitResult();


	float Damage = 0.0f;
	Damage += Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")));
	if (FName(TEXT("bip01_head")) == HitResult.BoneName)
	{
		Damage *= 2.f;
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGrizzlyAttributeSet::GetDamageAttribute(),EGameplayModOp::Additive,Damage));
}
