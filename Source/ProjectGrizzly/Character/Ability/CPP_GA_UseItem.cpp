// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_UseItem.h"
#include "..\..\Weapon\\WeaponComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "ProjectGrizzly/Character/CPP_Player.h"
#include "ProjectGrizzly/Item/CPP_Item.h"

UCPP_GA_UseItem::UCPP_GA_UseItem()
{
	//EGameplayAbilityTriggerSource : Enum
	// 	GameplayEvent	Gameplay Event �߻� �� �����Ƽ ����	�̺�Ʈ ��� ����, ȸ��, ���� ��
	// OwnedTagAdded	Ư�� �±װ� ĳ���Ϳ��� �߰��� �� ����	����(����), ���� ���� ��
	// OwnedTagPresent	Ư�� �±װ� �����Ǵ� ���� ����	�������� ����(�̼� ���� ��)
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem"));
	
	
	AbilityTriggers.Add(TriggerData);
}

void UCPP_GA_UseItem::InitVisualAsset(ACPP_Player* Player)
{
	if(!Item->GetItemData().ItemMesh.IsNull())
	{
		Player->GetUsableItemMeshComponent()->SetSkeletalMesh(Item->GetItemData().ItemMesh.LoadSynchronous());
	}
	if(!Item->GetItemData().AM_UseItem.IsNull())
	{
		AM_Item = Item->GetItemData().AM_UseItem.LoadSynchronous();
	}
	if(!Item->GetItemData().AM_Hands_UseItem.IsNull())
	{
		AM_Hands = Item->GetItemData().AM_Hands_UseItem.LoadSynchronous();
	}
}

void UCPP_GA_UseItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	if(!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	check(Player);
	SourceWeaponInstance = Player->GetCurrentWeaponInstance();

	// �� �ʱ�ȭ
	Item = Cast<ACPP_Item>(TriggerEventData->OptionalObject);
	if(!Item)
	{
		K2_EndAbility();
		return;
	}
	if(!Item->IsUsable() || Item->GetItemData().ItemEffects.IsEmpty())
	{
		K2_EndAbility();
		return;
	}
	ItemEffects = Item->GetItemData().ItemEffects;

	//���־� �ʱ�ȭ
	InitVisualAsset(Player);

	//������ ���¸�
	DoUndrawWeapon();
	
}

bool UCPP_GA_UseItem::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACPP_Player* Player = Cast<ACPP_Player>(ActorInfo->AvatarActor);
	if(!Player)
		return false;

	return true;
}

void UCPP_GA_UseItem::OnDrawWeaponCompleted(FGameplayEventData _Data)
{
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
	K2_EndAbility();
}

void UCPP_GA_UseItem::OnDrawWeaponStarted(FGameplayEventData _Data)
{
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
}

UAnimMontage* UCPP_GA_UseItem::GetCurrentWeaponUndrawAM() const
{
	UAnimMontage* AM = GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("Undraw"));
	checkf(AM,TEXT("Cant find Am"));
	return  AM;
}

UAnimMontage* UCPP_GA_UseItem::GetCurrentWeaponDrawAM() const
{
	UAnimMontage* AM = GetWeaponComponent()->Get_FP_Hands_Montage(TEXT("Draw"));
	checkf(AM,TEXT("Cant find Am"));
	return  AM;
}

void UCPP_GA_UseItem::DoUndrawWeapon()
{
	GetCharacter()->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(GetCurrentWeaponUndrawAM());
	auto* Task = UAbilityTask_WaitGameplayEvent::
		WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.SwapWeapon.OnUndrawCompleted")));
	Task->EventReceived.AddDynamic(this,&UCPP_GA_UseItem::OnUndrawWeaponCompleted);
	Task->ReadyForActivation();
	
	PrevLocation = GetCharacter()->GetIdleHandsLocation();
	PrevRotator = GetCharacter()->GetHandsRotation();
}

void UCPP_GA_UseItem::OnUndrawWeaponCompleted(FGameplayEventData _Data)
{
	DoDrawItem();
}

void UCPP_GA_UseItem::DoDrawItem()
{
	//������ ���ø���
	if(AM_Item && AM_Hands)
	{
		GetCharacter()->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(AM_Hands);
		auto* EndTask = UAbilityTask_WaitGameplayEvent::
			WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.UseItem.OnDrawCompleted")));
		EndTask->EventReceived.AddDynamic(this,&UCPP_GA_UseItem::OnDrawItemCompleted);
		EndTask->ReadyForActivation();

		ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
		Player->GetUsableItemMeshComponent()->PlayAnimation(AM_Item,false);

		auto* StartTask = UAbilityTask_WaitGameplayEvent::
			WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.UseItem.OnDrawStarted")));
		StartTask->EventReceived.AddDynamic(this,&UCPP_GA_UseItem::OnDrawItemStarted);
		StartTask->ReadyForActivation();

		//��ġ , �����̼� ����
		GetCharacter()->SetIdleHandsLocation({20.f,0,0});
		GetCharacter()->SetHandsRotation({0,-90.f,0});
	}


	//������ off
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
}

void UCPP_GA_UseItem::OnDrawItemCompleted(FGameplayEventData _Data)
{
	//���� ���ø���
	GetCharacter()->GetHandsMeshComponent()->GetAnimInstance()->Montage_Play(GetCurrentWeaponDrawAM());
	auto* EndTask = UAbilityTask_WaitGameplayEvent::
		WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.SwapWeapon.OnDrawCompleted")));
	EndTask->EventReceived.AddDynamic(this,&UCPP_GA_UseItem::OnDrawWeaponCompleted);
	EndTask->ReadyForActivation();
	

	auto* StartTask = UAbilityTask_WaitGameplayEvent::
		WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.SwapWeapon.OnDrawStarted")));
	StartTask->EventReceived.AddDynamic(this,&UCPP_GA_UseItem::OnDrawWeaponStarted);
	StartTask->ReadyForActivation();

	//��ġ , �����̼� ����
	GetCharacter()->SetIdleHandsLocation(PrevLocation);
	GetCharacter()->SetHandsRotation(PrevRotator);

	//������ off
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(false);
	
	
	//!!������ ����
}

void UCPP_GA_UseItem::OnDrawItemStarted(FGameplayEventData _Data)
{
	//GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
	
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(true);
}
