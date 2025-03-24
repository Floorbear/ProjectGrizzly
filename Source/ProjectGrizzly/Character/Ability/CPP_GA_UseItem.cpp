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
	// 	GameplayEvent	Gameplay Event 발생 시 어빌리티 실행	이벤트 기반 공격, 회복, 폭발 등
	// OwnedTagAdded	특정 태그가 캐릭터에게 추가될 때 실행	스턴(기절), 버프 적용 시
	// OwnedTagPresent	특정 태그가 유지되는 동안 실행	지속적인 버프(이속 증가 등)
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem"));

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")));
	
	//블록 : 사격, 기울이기, 재장전, 하이레디, 정조준
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Shoot")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Leaning")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.Reload")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.HighReady")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.ADS")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.SwapWeapon")));

	//캔슬 : 달리기, 기울이기, 사격, 재장전, 하이레디, 정조준
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

	// 값 초기화
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

	//비주얼 초기화
	InitVisualAsset(Player);

	if(GetCharacter()->IsUnarmed(SourceWeaponInstance))
	{
		DoDrawItem();
	}
	else
	{
		//무장한 상태면
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
	//위치 , 로테이션 셋팅
	GetCharacter()->SetIdleHandsLocation(PrevLocation);
	GetCharacter()->SetHandsRotation(PrevRotator);
	//렌더링
	if(GetCharacter()->IsUnarmed(SourceWeaponInstance))
	{
		//렌더링 off
		GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
		GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
	}
	else
	{
		//무기를 들었던 상태
		//렌더링 On
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
	//아이템 들어올리기
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

		//위치 , 로테이션 셋팅
		GetCharacter()->SetIdleHandsLocation({20.f,0,0});
		GetCharacter()->SetHandsRotation({0,-90.f,0});
	}


	//렌더링 off
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
}

void UCPP_GA_UseItem::OnDrawItemCompleted(FGameplayEventData _Data)
{
	//위치 , 로테이션 셋팅
	GetCharacter()->SetIdleHandsLocation(PrevLocation);
	GetCharacter()->SetHandsRotation(PrevRotator);
	
	//아이템 적용
	FGameplayEffectContextHandle Context = GetCharacter()->GetAbilitySystemComponent()->MakeEffectContext();
	for(int i =0; i < ItemEffects.Num(); i++)
	{
		FGameplayEffectSpecHandle SpecHandle = GetCharacter()->GetAbilitySystemComponent()->MakeOutgoingSpec(ItemEffects[i],GetAbilityLevel(),Context);
		
		GetCharacter()->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(),GetCharacter()->GetAbilitySystemComponent()->ScopedPredictionKey);
	}

	//아이템 인벤토리에서 삭제
	if(HasAuthority(&CurrentActivationInfo))
	{
		GetCharacter()->GetInventory()->RemoveItemFromInventory(Item->GetItemName(),1);
	}

	//렌더링 off
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(false);
	
	//무장상태
	if(!GetCharacter()->IsUnarmed(SourceWeaponInstance))
	{
		//무기 들어올리기
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
		//실행 흐름 : 클라이언트 To 서버
		//클라이언트에서 EndAbility를 했기 때문에
		//서버에서 OnDrawItemCompleted 실행하지않고 바로 EndAbility가 됨
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
