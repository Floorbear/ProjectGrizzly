// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// ���� ���� Get
#define LOG_LOCALROLEINFO  *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"),GetLocalRole()))
// ����Ʈ ���� Get
#define LOG_REMOTEROLEINFO  *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"),GetRemoteRole()))
// �ݸ�� Get
#define LOG_NETMODEINFO ((GetNetMode()==NM_Client)?*FString::Printf(TEXT("CLIENT%d"),GPlayInEditorID):(GetNetMode()==NM_Standalone)?TEXT("STANDALONE"):TEXT("SERVER"))
// ȣ���� �Լ��� Get
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
// [��Ʈ��ũ ���][���� ����/����Ʈ ����] ȣ���Լ� : �α׳���    ����
//#define Grizzly_LOG(Format, ...) UE_LOG(LogGrizzly, Warning, TEXT("[%s][%s/%s] %s  :  %s"),LOG_NETMODEINFO,LOG_LOCALROLEINFO,LOG_REMOTEROLEINFO,LOG_CALLINFO,*FString::Printf(Format,##__VA_ARGS__))
 #define Grizzly_LOG(Format, ...) UE_LOG(LogGrizzly, Warning, TEXT("%s  :  %s"),LOG_CALLINFO,*FString::Printf(Format,##__VA_ARGS__))
//��ũ�� ī�װ� ����
DECLARE_LOG_CATEGORY_EXTERN(LogGrizzly,Log,All)


UENUM(BlueprintType)
enum class EPGAbilityInputID : uint8
{
	//  None
	None			UMETA(DisplayName = "None"),
	//  Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	//  Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	//  LMB
	Shoot			UMETA(DisplayName = "Shoot"),
	//  RMB
	ADS				UMETA(DisplayName = "ADS"),
	//  R
	Reload			UMETA(DisplayName = "Reload"),
	// Shift
	Sprint			UMETA(DisplayName = "Sprint"),
	// C
	Crouch			UMETA(DisplayName = "Crouch"),
	//Leaning
	Leaning			UMETA(DisplayName = "Leaning"),

	Jump			UMETA(DisplayName = "Jump")
};