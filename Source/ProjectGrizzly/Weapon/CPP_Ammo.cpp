// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Ammo.h"

#include "IDetailTreeNode.h"


void UCPP_Ammo::Init(FName _RowName)
{
	Super::Init(_RowName);
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECaliber"), true);
	checkf(EnumPtr , TEXT("Invalid EnumPtr"));
	FString CaliberName = ItemData.NameWithOutPrefix.ToString();	

	//ECaliber의 Display Name과 아이템 DT의 이름이 동일한 것을 발견
	for(int i = 0; i <EnumPtr->NumEnums();++i)
	{
		bool Equal = EnumPtr->GetDisplayNameTextByIndex(i).ToString().Equals(CaliberName);
		if(Equal)
		{
			Caliber = static_cast<ECaliber>(EnumPtr->GetValueByIndex(i));
			return;
		}
	}
	checkf(false,TEXT("%s is not included in ECaliber Display Name"),*CaliberName);
}
