// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WeaponData.h"

#include <Curves\CurveVector.h>

#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIZZLY_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void InitWeaponData();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ----------------------------------------------------------------------
	//							Data Table Manage 
	// ----------------------------------------------------------------------
private:
	UPROPERTY()
	class UDataTable* WeaponDataTable;

	UPROPERTY()
	class UDataTable* WeaponAnimTable;

	struct FWeaponData* WeaponData;

	struct FWeaponAnim* WeaponAnim;

	class UCurveVector* WeaponRecoilCurve;

	EWeaponMode CurrentWeaponMode;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category  = "DataTable")
	FName WeaponName;

	class UCurveVector* GetWeaponRecoilCurve() const
	{
		ensure(WeaponRecoilCurve != nullptr);
		return WeaponRecoilCurve;
	}

	UFUNCTION(BlueprintCallable)
	float GetRecoilRecoveryTime() const
	{
		return WeaponData->RecoilRecoveryTime;
	}

	UFUNCTION(BlueprintCallable)
	float GetRPM() const
	{
		return WeaponData->RPM;
	}
	UFUNCTION(BlueprintCallable)
	float GetWeaponThreat() const
	{
		return WeaponData->Threat;
	}
	UFUNCTION(BlueprintCallable)
	float GetWeaponThreatRadius() const
	{
		return WeaponData->ThreatRadius;
	}
	UFUNCTION(BlueprintCallable)
	float GetRandomSpread() const
	{
		return WeaponData->RandomSpread;
	}
	UFUNCTION(BlueprintCallable)
	TSubclassOf<class UGameplayEffect> GetMuzzleFlashEffect()
	{
		return WeaponData->MuzzleFlashEffect;
	}

	UFUNCTION(BlueprintCallable)
	float GetLoudness() const
	{
		return WeaponData->Loudness;
	}

	UFUNCTION(BlueprintCallable)
	EWeaponMode GetCurrentWeaponMode() const
	{
		return CurrentWeaponMode;
	}
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ----------------------------------
	// ---------- Animation -------------
	// ----------------------------------
private:
	TMap<FName, UAnimMontage*> FP_Weapon_MontageMap;
	TMap<FName, UAnimMontage*> FP_Hands_MontageMap;
	TMap<FName, UAnimMontage*> TP_MontageMap;

	void LoadFPAnim(TSoftObjectPtr<UAnimMontage> _WeaponAnim, TSoftObjectPtr<UAnimMontage> _HandAnim, FName _Name);
public:
	UAnimMontage* Get_FP_Weapon_Montage(FName _Name) const;
	UAnimMontage* Get_FP_Hands_Montage(FName _Name) const;
	UAnimMontage* Get_TP_Montage(FName _Name) const;

private:
	// ToDo : Instance에 데이터 테이블 두기
	UFUNCTION(BlueprintCallable)
	class UDataTable*  GetWeaponDataTable() const;
	UFUNCTION(BlueprintCallable)
	class UDataTable*  GetWeaponAnimTable() const;

	// --------------------------------------------------------------------
	//								Magazine
	// --------------------------------------------------------------------
private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category  = "Magazine", meta = (AllowPrivateAccess = "true") , Replicated)
	int CurrentMagazineRounds;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category  = "Magazine", meta = (AllowPrivateAccess = "true"), Replicated)
	int RemainRounds;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category  = "Magazine", meta = (AllowPrivateAccess = "true"))
	int PerMagazineRounds;

public:
	UFUNCTION(BlueprintCallable)
	int GetCurrentMagazineRounds() const
	{
		return CurrentMagazineRounds;
	}
	UFUNCTION(BlueprintCallable)
	void SetCurrentMagazineRounds(int _NewValue);
	UFUNCTION(BlueprintCallable)
	int GetRemainRounds() const
	{
		return RemainRounds;
	}
	UFUNCTION(BlueprintCallable)
	void SetRemainRounds(int _NewValue);
	UFUNCTION(BlueprintCallable)
	int GetPerMagazineRounds() const
	{
		return PerMagazineRounds;
	}
	UFUNCTION(BlueprintCallable)
	bool IsMagazineEmpty() const
	{
		if (CurrentMagazineRounds <= 0)
		{
			return true;
		}

		return false;
	}
	UFUNCTION(BlueprintCallable)
	bool IsMagazineFull() const
	{
		//현재 구조에서 모든 총에 약실이 있다고 가정함 소드오프 샷건, 리볼버 같은 구조의 총은 추가 X
		if (CurrentMagazineRounds == PerMagazineRounds + 1)
		{
			return true;
		}

		return false;
	}

	void Reload_Inner();
	UFUNCTION(Reliable,Server)
	void Reload_RPC();
	UFUNCTION(BlueprintCallable)
	void Reload()
	{
		if (GetOwner()->HasAuthority())
		{
			Reload_Inner();
		}
		else
		{
			Reload_RPC();
		}
	}
};
