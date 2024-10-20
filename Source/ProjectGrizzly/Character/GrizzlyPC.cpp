// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyPC.h"
#include "ProjectGrizzly/ProjectGrizzly.h"

AGrizzlyPC::AGrizzlyPC()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
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
	Grizzly_LOG(TEXT("Begin"));
	Super::OnPossess(aPawn);
	Grizzly_LOG(TEXT("End"));
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
