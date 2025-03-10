// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GA_UseItem.h"
#include "..\..\Weapon\\WeaponComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "ProjectGrizzly/Character/CPP_Player.h"
#include "ProjectGrizzly/Item/CPP_Item.h"

UCPP_GA_UseItem::UCPP_GA_UseItem()
{
	//EGameplayAbilityTriggerSource : Enum
	// 	GameplayEvent	Gameplay Event 발생 시 어빌리티 실행	이벤트 기반 공격, 회복, 폭발 등
	// OwnedTagAdded	특정 태그가 캐릭터에게 추가될 때 실행	스턴(기절), 버프 적용 시
	// OwnedTagPresent	특정 태그가 유지되는 동안 실행	지속적인 버프(이속 증가 등)
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

	//무장한 상태면
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

	//위치 , 로테이션 셋팅
	GetCharacter()->SetIdleHandsLocation(PrevLocation);
	GetCharacter()->SetHandsRotation(PrevRotator);

	//렌더링 off
	GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(false);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(false);
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(false);
	
	
	//!!아이템 적용
}

void UCPP_GA_UseItem::OnDrawItemStarted(FGameplayEventData _Data)
{
	//GetCharacter()->GetFPWeaponMeshComponent()->SetVisibility(true);
	GetCharacter()->GetHandsMeshComponent()->SetVisibility(true);
	
	ACPP_Player* Player = Cast<ACPP_Player>(GetCharacter());
	Player->GetUsableItemMeshComponent()->SetVisibility(true);
}
