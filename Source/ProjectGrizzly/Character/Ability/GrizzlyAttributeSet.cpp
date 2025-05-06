// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include <GameFramework\Character.h>
#include <Kismet/GameplayStatics.h>
#include "..\CPP_PGCharacter.h"

UGrizzlyAttributeSet::UGrizzlyAttributeSet()
{

}

void UGrizzlyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGrizzlyAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGrizzlyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGrizzlyAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGrizzlyAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGrizzlyAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGrizzlyAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
}

//���������� ���� ��
//��Ʈ����Ʈ ���� ����Ǹ� ȣ��Ǵ� �ݹ� �Լ�
void UGrizzlyAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//����� ���� ���������
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		UAbilitySystemComponent* Source = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
		ACPP_PGCharacter* SourceActor = Cast<ACPP_PGCharacter>(Source->AvatarActor);
		AController* SourceController = SourceActor->GetController();


		//ToDo : HitResult�� �̿��� �߰� ���� ����
		float LocalDamage = Data.EvaluatedData.Magnitude;;
		SetDamage(0.f);
		if (LocalDamage > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth() - LocalDamage, 0.f, GetMaxHealth()));
		}
		//�������� ���� HP���Ҹ� ������ �̺�Ʈ���� ������ ó���Ѵ�
		const FHitResult* HitResult = Data.EffectSpec.GetContext().GetHitResult();
		if(HitResult)
		{
			TSubclassOf<UDamageType> DamageType;
			UGameplayStatics::ApplyPointDamage(Data.Target.AvatarActor,LocalDamage,HitResult->ImpactNormal,*HitResult,SourceController,SourceActor, DamageType);
		}

	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(),0,GetMaxStamina()));
		/*ACPP_PGCharacter* Character = Cast<ACPP_PGCharacter>(Source->AvatarActor);
		Character->SetSpeed(GetSpeed());*/
	}
}

void UGrizzlyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGrizzlyAttributeSet, Health, OldHealth);
}

void UGrizzlyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGrizzlyAttributeSet, MaxHealth, OldMaxHealth);
}

void UGrizzlyAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGrizzlyAttributeSet, Damage, OldDamage);
}

void UGrizzlyAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGrizzlyAttributeSet, Speed, OldSpeed);
}

void UGrizzlyAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGrizzlyAttributeSet, Stamina, OldStamina);
}

void UGrizzlyAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGrizzlyAttributeSet, MaxStamina, OldMaxStamina);
}
