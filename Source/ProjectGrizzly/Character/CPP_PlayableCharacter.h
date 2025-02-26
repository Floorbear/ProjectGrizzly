// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_PGCharacter.h"
#include <Engine/DataTable.h>

#include "ProjectGrizzly/Item/Interactable.h"
#include "ProjectGrizzly/Weapon/CPP_WeaponInstance.h"
#include "CPP_PlayableCharacter.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class ECharacterModel : uint8
{
	None				UMETA(DisplayName = "None"),
	Merc_2a				UMETA(DisplayName = "Merc_2a"),
	Bandit_3a			UMETA(DisplayName = "Bandit_3a")
};

USTRUCT(BlueprintType)
struct FCharacterModel : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> Character;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> Hands;
};


UCLASS()
class PROJECTGRIZZLY_API ACPP_PlayableCharacter : public ACPP_PGCharacter , public IInteractable
{
	GENERATED_BODY()

public:
	ACPP_PlayableCharacter();

	virtual  void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdateADS(float DeltaTime);

protected:
	void BeginPlay() override;



	// ------------------------------------------------
	//					Visual Model
	// ------------------------------------------------
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VisualModel, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_SetCharacterModel)
	ECharacterModel CharacterModel = ECharacterModel::None;
	UFUNCTION()
	void OnRep_SetCharacterModel();
	UFUNCTION(Server, Reliable)
	void SetCharacterModel_Server(ECharacterModel _Model);
public:
	UFUNCTION(BlueprintCallable)
	void SetCharacterModel(ECharacterModel _Model);

	// ----------------------------------
	//			Shadow
	// ----------------------------------
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShadowModelComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShadowWeaponComponent;
public:
	UFUNCTION(BlueprintCallable, Category = Shadow)
	USkeletalMeshComponent* GetShadowWeaponComponent() const
	{
		return ShadowWeaponComponent;
	}
	// ----------------
	// ----- GAS ------
	// ----------------
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGrizzlyAbility>> PlayerAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> StartEffects;

	bool ASCInputBound = false;


	// ------------------
	// ----- Camera -----
	// ------------------
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float MouseXDelta = 0;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float MouseYDelta = 0;

	UFUNCTION(BlueprintCallable)
	class UCameraComponent* GetCamera() const;

	// ----------------------------------------------------------------
	//				 First Person Hands			
	// ----------------------------------------------------------------
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hands", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* HandsMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hands", meta = (AllowPrivateAccess = "true"))
	float ADSFactor = 0.f; // 0 : IdleHand , 1 : ADSHand
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hands", meta = (AllowPrivateAccess = "true"))
	bool bADS = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands", meta = (AllowPrivateAccess = "true"))
	FVector IdleHandLocation = { 3.8f,6.f,-1.8f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands", meta = (AllowPrivateAccess = "true"))
	FVector ADSHandsLocation = { 0,0,0 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands", meta = (AllowPrivateAccess = "true"))
	FRotator IdleHandRotation = { 0,-90.f,0 };
public:
	UFUNCTION(BlueprintCallable)
	FVector GetIdleHandsLocation() const
	{
		return IdleHandLocation;
	}
	UFUNCTION(BlueprintCallable)
	FVector GetADSHandsLocation() const
	{
		return ADSHandsLocation;
	}
	UFUNCTION(BlueprintCallable)
	FRotator GetHandsRotation() const
	{
		return IdleHandRotation;
	}
	UFUNCTION(BlueprintCallable)
	void SetIdleHandsLocation(FVector _Location)
	{
		IdleHandLocation = _Location;
	}
	UFUNCTION(BlueprintCallable)
	void SetADSHandsLocation(FVector _Location)
	{
		ADSHandsLocation = _Location;
	}
	UFUNCTION(BlueprintCallable)
	void SetHandsRotation(FRotator _Rotator)
	{
		IdleHandRotation = _Rotator;
	}
	
public:
	// 소규모 게임이므로 프로퍼티 리플리케이션 X
	// 클라이언트에서만 필요한 데이터지만 대규모 게임에서 타당성 검증을 해야함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	bool bHighReady = false;
public:
	UFUNCTION(BlueprintCallable)
	class USkeletalMeshComponent* GetHandsMeshComponent();

	UFUNCTION(BlueprintCallable, Category = "Hands")
	float GetADSSpeed();
	UFUNCTION(BlueprintCallable, Category = "Hands")
	void SetADS(bool _NewBool);
	UFUNCTION(BlueprintCallable, Category = "Hands")
	bool GetADS() const;

	UFUNCTION(BlueprintCallable, Category = "Hands")
	float GetADSFactor() const; // 0 : IdleHand , 1 : ADSHand

	UFUNCTION(BlueprintCallable, Category = "Hands")
	void ReturnToHands(float _DeltaTime);

	// ADS 상태 ㅇ: ADS 로케이션 리턴 
	// ADS 상태 X : Idle 로캐이션 리턴
	UFUNCTION(BlueprintCallable, Category = "Hands")
	FVector GetCurrentHandsLocation() const
	{
		if (bADS)
		{
			return GetADSHandsLocation();
		}
		return GetIdleHandsLocation();
	}

	// ----------------------------------
	//			Damage
	// ----------------------------------
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// ----------------------------------
	//			Die
	// ----------------------------------
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Die", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DeadGameplayEffect;
	FVector DeadLocation;
public:
	void Die() override;
	UFUNCTION(NetMulticast, Reliable)
	void Die_Multicast();

	virtual bool IsDead() const override;
	// ----------------------------------
	//		 Third Person Cam
	// ----------------------------------
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPCam", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TPCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPCam", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* TPSpringArm;

	void UpdateDeathCam(float _DeltaTime);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPCam", meta = (AllowPrivateAccess = "true"))
	float DeathCamSpeed = 15.f;



	// ----------------------------------
	//			Bending & Leaning
	// ----------------------------------
private:
	// 순수한 Axis 값. 기울이기 수치값을 전달 받기위해서는 GetLeaningAxis 를 사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RawLeaningAxis = 0.f;

	// Leaning 어빌리티의 LeaningAxis를 감시하는 값
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LeaningAxis;
public:
	UFUNCTION(BlueprintCallable)
	float GetLeaningAxis() const;
	UFUNCTION(BlueprintCallable)
	void SetLeaningAxis(float _LeaningAxis);
	UFUNCTION(BlueprintCallable)
	float GetRawLeaningAxis() const
	{
		return RawLeaningAxis;
	}
	UFUNCTION()
	void SetRawLeaningAxis(float _LeaningAxis);
private:
	UFUNCTION(Server, Unreliable)
	void SetRawLeaningAxis_Server(float _LeaningAxis);

public:
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float BendDownDegree = 0.f;
	UFUNCTION(BlueprintCallable)
	virtual void UpdateBendDownDegree();
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	virtual void SetBendDownDegree_Server(float _Degree);

	//--------------------------------------------------------------------------------------------------
	//										Weapon
	//--------------------------------------------------------------------------------------------------
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UWeaponComponent* WeaponComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TPWeaponComponent;

public:
	UFUNCTION(BlueprintCallable)
	class UWeaponComponent* GetWeaponComponent() const;
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetTPWeaponComponent() const;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FPWeaponMeshComponent;

public:
	UFUNCTION(BlueprintCallable)
	class USkeletalMeshComponent* GetFPWeaponMeshComponent();

	//--------------------------------------------------------------------------------------------------
	//										WeaponSlot 
	//--------------------------------------------------------------------------------------------------
public:
	EWeaponSlot GetCurrentWeaponSlot() const
	{
		return static_cast<EWeaponSlot>(CurrentWeaponSlot);
	}
	UFUNCTION(Server,Reliable)
	void SetCurrentWeaponSlot(int _CurrentWeaponSlot);

	virtual ACPP_WeaponInstance* GetCurrentWeaponInstance(){return nullptr;}
	virtual ACPP_WeaponInstance* GetWeaponInstanceFromSlot(EWeaponSlot _Slot){return nullptr;}
	virtual bool IsUnarmed(ACPP_WeaponInstance* _WeaponInstance) const;
	UFUNCTION(BlueprintCallable,Category = "WeaponSlot")
	virtual void TryActivateSwapWeapon(EWeaponSlot _Slot){};
	virtual void SwapWeapon(EWeaponSlot _Slot){};
protected:
	UPROPERTY(Replicated)
	int CurrentWeaponSlot = 0;

	//--------------------------------------------------------------------------------------------------
	//										Inventory
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	class UPlayerInventoryComponent* GetInventory() const
	{
		return InventoryComponent;
	}

private:
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category = "Inventory",meta = (AllowPrivateAccess = true))
	class UPlayerInventoryComponent* InventoryComponent;
	// ------------------------------
	//				AI
	// ------------------------------
public:
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = "AI")
	bool bIsAI = false;

	//--------------------------------------------------------------------------------------------------
	//										Interact
	//--------------------------------------------------------------------------------------------------
protected:
	virtual bool CanInteract_Implementation() const override
	{
		return true;
	}

	virtual void Interact_Implementation(class AActor* _Instigator) override;

	virtual void ShowWidget_Implementation() override{};
	virtual void CloseWidget_Implementation() override{};
};
