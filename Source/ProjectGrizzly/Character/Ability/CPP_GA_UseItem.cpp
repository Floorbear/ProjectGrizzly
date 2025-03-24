// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_UseItem.h"
#include "..\..\Weapon\\WeaponComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "ProjectGrizzly/Character/CPP_Player.h"
#include "ProjectGrizzly/Character/PlayerInventoryComponent.h"
#include "ProjectGrizzly/Item/CPP_Item.h"

UCPP_GA_UseItem::UCPP_GA_UseItem()
{
	//EGameplayAbilityTriggerSource : Enum
	// 	GameplayEvent	Gameplay Event �߻� �� �����Ƽ ����	�̺�Ʈ ��� ����, ȸ��, ���� ��
	// OwnedTagAdded	Ư�� �±װ� ĳ���Ϳ��� �߰��� �� ����	����(����), ���� ���� ��
	// OwnedTagPresent	Ư�� �±װ� �����Ǵ� ���� ����	�������� ����(�̼� ���� ��)
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem"));

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")));
	
	//��� : ���, ����̱�, ������, ���̷���, ������
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Shoot")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Leaning")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Reload")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.HighReady")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.SwapWeapon")));

	//ĵ�� : �޸���, ����̱�, ���, ������, ���̷���, ������
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Shoot")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Leaning")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Sprint")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Reload")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.HighReady")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	
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
	PrevLocation = GetCharacter()->GetIdleHandsLocation();
	PrevRotator = GetCharacter()->GetHandsRotation();

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

	if(GetCharacter()->IsUnarmed(SourceWeaponInstance))
	{
		DoDrawItem();
	}
	else
	{
		//������ ���¸�
		DoUndrawWeapon();
	}

	
}

void UCPP_GA_UseItem::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if(CanBeCanceled())
	{
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

		ACPP_PlayableCharacter* Player = Cast<ACPP_PlayableCharacter>(CurrentActorInfo->AvatarActor);

		if(AM_Hands)
		{
			Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_StopWithBlendOut(0.1f,AM_Hands);
		}


	}
}

void UCPP_GA_UseItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(false);
	if(AM_Hands)
	{
		Player->GetHandsMeshComponent()->GetAnimInstance()->Montage_StopWithBlendOut(0.1f,AM_Hands);
	}
	//��ġ , �����̼� ����
	GetCharacter()->SetIdleHandsLocation(PrevLocation);
	GetCharacter()->SetHandsRotation(PrevRotator);
	//������
	if(GetCharacter()->IsUnarmed(SourceWeaponInstance))
	{
		//������ off
		GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
		GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
	}
	else
	{
		//���⸦ ����� ����
		//������ On
		GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
		GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
	}
	Item = nullptr;
	ItemEffects.Empty();
	SourceWeaponInstance = nullptr;
	AM_Hands = nullptr;
	AM_Item = nullptr;


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
	//��ġ , �����̼� ����
	GetCharacter()->SetIdleHandsLocation(PrevLocation);
	GetCharacter()->SetHandsRotation(PrevRotator);
	
	//������ ����
	FGameplayEffectContextHandle Context = GetCharacter()->GetAbilitySystemComponent()->MakeEffectContext();
	for(int i =0; i < ItemEffects.Num(); i++)
	{
		FGameplayEffectSpecHandle SpecHandle = GetCharacter()->GetAbilitySystemComponent()->MakeOutgoingSpec(ItemEffects[i],GetAbilityLevel(),Context);
		
		GetCharacter()->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(),GetCharacter()->GetAbilitySystemComponent()->ScopedPredictionKey);
	}

	//������ �κ��丮���� ����
	if(HasAuthority(&CurrentActivationInfo))
	{
		GetCharacter()->GetInventory()->RemoveItemFromInventory(Item->GetItemName(),1);
	}

	//������ off
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(false);
	
	//�������
	if(!GetCharacter()->IsUnarmed(SourceWeaponInstance))
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
	}
	else
	{
		//���� �帧 : Ŭ���̾�Ʈ To ����
		//Ŭ���̾�Ʈ���� EndAbility�� �߱� ������
		//�������� OnDrawItemCompleted ���������ʰ� �ٷ� EndAbility�� ��
		if(HasAuthority(&CurrentActivationInfo))
		{
			K2_EndAbility();
		}
	}


}

void UCPP_GA_UseItem::OnDrawItemStarted(FGameplayEventData _Data)
{
	//GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(true);
}
