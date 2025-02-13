// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\Item\CPP_Item.h"
#include "WeaponData.h"
#include "CPP_Ammo.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API ACPP_Ammo : public ACPP_Item
{
	GENERATED_BODY()
public:
	ECaliber GetCaliber() const
	{
		return  Caliber;
	}
	void SetCaliber(ECaliber _Caliber)
	{
		Caliber = _Caliber;
	}
	virtual void Init(FName _RowName) override;
private:
	ECaliber Caliber;
};
