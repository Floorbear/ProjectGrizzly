// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WeaponData.h"

#include <Curves\CurveVector.h>

#include "CPP_WeaponInstance.h"
#include "CPP_Ammo.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIZZLY_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UCPP_GA_SwapWeapon;
public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//--------------------------------------------------------------------------------------------------
//										Set Weapon
//--------------------------------------------------------------------------------------------------

public:
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void SetWeapon(ACPP_WeaponInstance* _WeaponInstance);
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void SetWeaponWithAmmo(ACPP_WeaponInstance* _WeaponInstance , ACPP_Ammo* _Ammo);
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void SetUnarmed();
	ACPP_WeaponInstance* GetUnarmedInstance() const
	{
		return UnarmedWeaponInstance;
	}

	//--------------------------------------------------------------------------------------------------
	//										Weapon Instance
	//--------------------------------------------------------------------------------------------------
private:
	void SetWeapon_Inner(ACPP_WeaponInstance* _WeaponInstance);
	
	UFUNCTION()
	void OnRep_CurrentWeaponInstance();
	//CurrentWeaponInstance의 소유권은 PlayerInventory에 있음
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponInstance)
	ACPP_WeaponInstance* CurrentWeaponInstance = nullptr;
	//UnarmedWeaponInstance는 WeaponComponent에서 생성하니 여기에 있음
	UPROPERTY(Replicated)
	ACPP_WeaponInstance* UnarmedWeaponInstance = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	ACPP_WeaponInstance* GetCurrentWeaponInstance() const
	{
		checkf(CurrentWeaponInstance != nullptr,TEXT("CurrentWeaponInstance is Null"));
		return CurrentWeaponInstance;
	}


	// ----------------------------------------------------------------------
	//							Data Table Manage 
	// ----------------------------------------------------------------------
private:
	UDataTable* WeaponDataDT = nullptr;
	UDataTable* WeaponAnimDT = nullptr;
	

	class UCurveVector* WeaponRecoilCurve;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category  = "DataTable")
	FName CurrentWeaponName;


	
	class UCurveVector* GetWeaponRecoilCurve() const
	{
		ensure(WeaponRecoilCurve != nullptr);
		return WeaponRecoilCurve;
	}

	FWeaponData* GetCurrentWeaponData() const;
	FWeaponAnim* GetCurrentAnimData() const;
	UFUNCTION(BlueprintCallable)
	float GetRecoilRecoveryTime() const
	{
		return GetCurrentWeaponData()->RecoilRecoveryTime;
	}

	UFUNCTION(BlueprintCallable)
	float GetRPM() const
	{
		return GetCurrentWeaponData()->RPM;
	}
	UFUNCTION(BlueprintCallable)
	float GetWeaponThreat() const
	{
		return GetCurrentWeaponData()->Threat;
	}
	UFUNCTION(BlueprintCallable)
	float GetWeaponThreatRadius() const
	{
		return GetCurrentWeaponData()->ThreatRadius;
	}
	UFUNCTION(BlueprintCallable)
	float GetRandomSpread() const
	{
		return GetCurrentWeaponData()->RandomSpread;
	}
	UFUNCTION(BlueprintCallable)
	TSubclassOf<class UGameplayEffect> GetMuzzleFlashEffect()
	{
		return GetCurrentWeaponData()->MuzzleFlashEffect;
	}

	UFUNCTION(BlueprintCallable)
	float GetLoudness() const
	{
		return GetCurrentWeaponData()->Loudness;
	}

	UFUNCTION(BlueprintCallable)
	EWeaponMode GetCurrentWeaponMode() const
	{
		if(CurrentWeaponInstance == nullptr)
			return EWeaponMode::SemiAuto;
		return CurrentWeaponInstance->GetCurrentWeaponMode();
	}

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const
	{
		return GetCurrentWeaponData()->WeaponType;
	}
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ----------------------------------------------------------------------------------------------------
	//										Animation
	// ----------------------------------------------------------------------------------------------------

private:
	TMap<FName, UAnimMontage*> FP_Weapon_MontageMap;
	TMap<FName, UAnimMontage*> FP_Hands_MontageMap;
	TMap<FName, UAnimMontage*> TP_MontageMap;

	void InitTPAnim();
	void LoadFPAnim(TSoftObjectPtr<UAnimMontage> _WeaponAnim, TSoftObjectPtr<UAnimMontage> _HandAnim, FName _Name);
public:
	UAnimMontage* Get_FP_Weapon_Montage(FName _Name) const;
	UAnimMontage* Get_FP_Hands_Montage(FName _Name) const;
	//bCombatAnim : _Name 뒤에 _WeaponType 접미사를 붙인다. 전투 몽타주에 사용
	UFUNCTION(BlueprintCallable)
	UAnimMontage* Get_TP_Montage(FName _Name) const;

private:
	// ToDo : Instance에 데이터 테이블 두기
	UFUNCTION(BlueprintCallable)
	class UDataTable*  GetWeaponDataDT() const;
	UFUNCTION(BlueprintCallable)
	class UDataTable*  GetWeaponAnimDT() const;

public:
	bool HasEmptyIdleAnim() const;
	UFUNCTION(BlueprintCallable,NetMulticast,Reliable)
	void SetEmptyIdleAnim();

	UFUNCTION(BlueprintCallable,NetMulticast,Reliable)
	void SetAnimIdle();
	// --------------------------------------------------------------------
	//								Magazine
	// --------------------------------------------------------------------
private:
	UPROPERTY(Replicated)
	ACPP_Ammo* AmmoInstance = nullptr;

public:
	UFUNCTION(Server,Reliable,Category = "Network")
	void SetAmmoInstance(ACPP_Ammo* _AmmoInstance);
	
	
	ACPP_Ammo* GetAmmoInstance() const
	{
		if(!IsValid(AmmoInstance))
		{
			return nullptr;
		}
		return AmmoInstance;
	}
	
	// UPROPERTY(EditAnywhere,BlueprintReadWrite,Category  = "Magazine", meta = (AllowPrivateAccess = "true"))
	// int PerMagazineRounds;

public:
	UFUNCTION(BlueprintCallable)
	int GetCurrentMagazineRounds() const
	{
		if(CurrentWeaponInstance == nullptr)
			return 0;
		return CurrentWeaponInstance->GetRounds();
	}
	UFUNCTION(BlueprintCallable)
	void SetCurrentMagazineRounds(int _NewValue);
	UFUNCTION(BlueprintCallable)
	int GetRemainRounds() const
	{
		if(AmmoInstance == nullptr)
		{
			return 0;
		}
		return AmmoInstance->GetAmount();
	}
	UFUNCTION(BlueprintCallable)
	int GetPerMagazineRounds() const
	{
		return CurrentWeaponInstance->GetWeaponData()->Rounds;
	}
	UFUNCTION(BlueprintCallable)
	bool IsMagazineEmpty() const
	{
		if (GetCurrentMagazineRounds() <= 0)
		{
			return true;
		}

		return false;
	}
	UFUNCTION(BlueprintCallable)
	bool IsMagazineFull() const
	{
		//현재 구조에서 모든 총에 약실이 있다고 가정함 소드오프 샷건, 리볼버 같은 구조의 총은 추가 X
		if (GetCurrentMagazineRounds() == GetPerMagazineRounds() + 1)
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

	//--------------------------------------------------------------------------------------------------
	//										Network
	//--------------------------------------------------------------------------------------------------

	virtual void InitializeComponent() override;
};
