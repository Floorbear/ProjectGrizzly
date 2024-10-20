// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyGameMode.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
void AGrizzlyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Grizzly_LOG(TEXT("%s"), TEXT("==========New Client Try to Connet==================="));
	Super::PreLogin(Options,Address,UniqueId,ErrorMessage);

}

APlayerController* AGrizzlyGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Grizzly_LOG(TEXT("%s"), TEXT("Begin"));
	APlayerController* NewController = Super::Login(NewPlayer,InRemoteRole,Portal,Options,UniqueId,ErrorMessage);
	Grizzly_LOG(TEXT("%s"), TEXT("End"));
	return NewController;
}

void AGrizzlyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	bool bExample = false;
	//넷 드라이버를 가져온다
	if (bExample)
	{
		UNetDriver* NetDriver = GetNetDriver();
		if (NetDriver)
		{
			//넷드라이버에 클라이언트 커넥션이 존재하지 않는다
			if (NetDriver->ClientConnections.Num() == 0)
			{
				Grizzly_LOG(TEXT("No Client Connections"));
			}
			//넷드라이버에 클라이언트 커넥션이 존재한다
			else
			{
				//각 커넥션의 이름을 로그에 찍는다
				for (auto& Connection : NetDriver->ClientConnections)
				{
					Grizzly_LOG(TEXT("Client Connections : %s"), *Connection->GetName());
				}
			}
		}
		else
		{
			Grizzly_LOG(TEXT("No NetDriver"));
		}
	}

}

void AGrizzlyGameMode::StartPlay()
{
	Super::StartPlay();
}
