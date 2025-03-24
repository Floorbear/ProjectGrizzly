// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyPC.h"

#include "Net/UnrealNetwork.h"
#include "ProjectGrizzly/ProjectGrizzly.h"

AGrizzlyPC::AGrizzlyPC()
{
}

void AGrizzlyPC::BeginPlay()
{
	Super::BeginPlay();

}

void AGrizzlyPC::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGrizzlyPC::PostNetInit()
{
	Super::PostNetInit();
	UNetDriver* NetDriver = GetNetDriver();

	if (NetDriver)
	{
		// ��Ʈ��ũ ��尡 Ŭ���̾�Ʈ�� ���� Ŀ�ؼ��� �����Ѵ�
		if (NetDriver->ServerConnection)
		{
			//Grizzly_LOG(LogGrizzly, Log, TEXT("Server Connections : %s"), *NetDriver->ServerConnection->GetName());
		}
	}
	//��� PostNetInit�Լ��� Ŭ���̾�Ʈ������ ȣ��ȴ�
	// Ŭ���̾�Ʈ�� �ݵ���̹��� ������ ������
	// �׷��� �� else�κ��� ȣ��� ���� ���� �ϴ�
	else
	{
		//Grizzly_LOG(LogGrizzly, Log, TEXT("No NetDriver"));
	}
}

void AGrizzlyPC::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	// ACPP_PlayableCharacter* PossessCharacter= Cast<ACPP_PlayableCharacter>(aPawn);
	// check(PossessCharacter);
	// PossessCharacter->GetInventory()->InitWeaponInstanceToUnarmedInstance();
}

bool AGrizzlyPC::IsMyComputer()
{
	if (GetNetMode() == NM_Standalone)
	{
		return true;
	}
	else if (GetNetMode() == NM_Client)
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			return true;
		}
	}
	return false;
}

UPlayerInventoryComponent* AGrizzlyPC::GetInventoryComponent() const
{
	ACPP_PlayableCharacter* PossessCharacter= Cast<ACPP_PlayableCharacter>(GetPawn());
	check(PossessCharacter);

	return PossessCharacter->GetInventory();
}

void AGrizzlyPC::LoadPlayerInventoryFromInstance()
{
	if(GetNetMode() == NM_DedicatedServer)
	{
		Grizzly_LOG(TEXT("LoadPlayerInventoryFromInstance Function is supposed to called by Client . Not Server"));
		return;
	}
	UGrizzlyGameInstance* GameInstance =Cast<UGrizzlyGameInstance>(GetGameInstance());
	if(GameInstance->GetGamePhase() != EGamePhase::Playing)
	{
		Grizzly_LOG(TEXT("LoadPlayerInventoryFromInstance Function is supposed to called when Playing Phase ."));
		return;
	}
	const FInventoryData& InventoryData = GameInstance->GetInventoryFromKey(TEXT("Player"));
	GetInventoryComponent()->SetInventory(InventoryData);
	// if(ACPP_WeaponInstance* PrimaryWeapon = 	GetInventoryComponent()->FindWeaponInstanceFromSlot(EWeaponSlot::Primary); PrimaryWeapon)
	// {
	// 	GetInventoryComponent()->EquipWeapon(PrimaryWeapon,EWeaponSlot::Primary);
	// }
	// if(ACPP_WeaponInstance* SecondaryWeapon = 	GetInventoryComponent()->FindWeaponInstanceFromSlot(EWeaponSlot::Secondary); SecondaryWeapon)
	// {
	// 	GetInventoryComponent()->EquipWeapon(SecondaryWeapon,EWeaponSlot::Secondary);
	// }

}

void AGrizzlyPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

EGamePhase AGrizzlyPC::GetGamePhase() const
{
	UGrizzlyGameInstance* GameInstance = Cast<UGrizzlyGameInstance>(GetGameInstance());
	return GameInstance->GetGamePhase();
}

void AGrizzlyPC::SetGamePhase_Implementation(EGamePhase _GamePhase)
{
	UGrizzlyGameInstance* GameInstance = Cast<UGrizzlyGameInstance>(GetGameInstance());
	GameInstance->SetGamePhase(_GamePhase);
}
