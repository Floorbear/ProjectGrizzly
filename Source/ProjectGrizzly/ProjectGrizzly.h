// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 로컬 역할 Get
#define LOG_LOCALROLEINFO  *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"),GetLocalRole()))
// 리모트 역할 Get
#define LOG_REMOTEROLEINFO  *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"),GetRemoteRole()))
// 넷모드 Get
#define LOG_NETMODEINFO ((GetNetMode()==NM_Client)?*FString::Printf(TEXT("CLIENT%d"),GPlayInEditorID):(GetNetMode()==NM_Standalone)?TEXT("STANDALONE"):TEXT("SERVER"))
// 호출한 함수명 Get
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
// [네트워크 모드][로컬 역할/리모트 역할] 호출함수 : 로그내용    서식
//#define Grizzly_LOG(Format, ...) UE_LOG(LogGrizzly, Warning, TEXT("[%s][%s/%s] %s  :  %s"),LOG_NETMODEINFO,LOG_LOCALROLEINFO,LOG_REMOTEROLEINFO,LOG_CALLINFO,*FString::Printf(Format,##__VA_ARGS__))
 #define Grizzly_LOG(Format, ...) UE_LOG(LogGrizzly, Warning, TEXT("%s  :  %s"),LOG_CALLINFO,*FString::Printf(Format,##__VA_ARGS__))
//매크로 카테고리 생성
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