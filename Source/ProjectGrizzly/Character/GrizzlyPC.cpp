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
		// 네트워크 모드가 클라이언트면 서버 커넥션이 존재한다
		if (NetDriver->ServerConnection)
		{
			//Grizzly_LOG(LogGrizzly, Log, TEXT("Server Connections : %s"), *NetDriver->ServerConnection->GetName());
		}
	}
	//사실 PostNetInit함수는 클라이언트에서만 호출된다
	// 클라이언트는 넷드라이버를 무조건 가진다
	// 그래서 이 else부분이 호출될 경우는 없긴 하다
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
