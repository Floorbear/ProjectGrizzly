// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_PGCharacter.h"
#include "CPP_PlayableCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIZZLY_API ACPP_PlayableCharacter : public ACPP_PGCharacter
{
	GENERATED_BODY()

public:
	ACPP_PlayableCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void Tick(float DeltaTime) override;

	void UpdateADS(float DeltaTime);

protected:
	void BeginPlay() override;




	// ----------------
	// ----- GAS ------
	// ----------------
	protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGrizzlyAbility>> PlayerAbilities;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> StartEffects;

	bool ASCInputBound = false;
	

	// ------------------
	// ----- Camera -----
	// ------------------
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

public:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "Camera")
	float MouseXDelta = 0;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category = "Camera")
	float MouseYDelta = 0;

	UFUNCTION(BlueprintCallable)
	class UCameraComponent* GetCamera() const;


	// ----------------------------------
	// ----- First Person Hands ---------
	// ----------------------------------
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Hands", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* HandsMeshComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Hands", meta = (AllowPrivateAccess = "true"))
	float ADSFactor = 0.f; // 0 : IdleHand , 1 : ADSHand
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Hands", meta = (AllowPrivateAccess = "true"))
	bool bADS = false;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Hands",meta = (AllowPrivateAccess = "true"))
	FVector IdleHandLocation = {3.8f,6.f,-1.8f};
public:
	UFUNCTION(BlueprintCallable)
	class USkeletalMeshComponent* GetHandsMeshComponent();

	UFUNCTION(BlueprintCallable , Category = "Hands")
	float GetADSSpeed();
	UFUNCTION(BlueprintCallable , Category = "Hands")
	void SetADS(bool _NewBool);
	UFUNCTION(BlueprintCallable , Category = "Hands")
	bool GetADS() const;

	UFUNCTION(BlueprintCallable , Category = "Hands") 
	float GetADSFactor() const; // 0 : IdleHand , 1 : ADSHand

	// ----------------------------------
	// ---------------Die----------------
	// ----------------------------------
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Die", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DeadGameplayEffect;
	FVector DeadLocation;
public:
	void Die() override;
	UFUNCTION(NetMulticast,Reliable)
	void Die_Multicast();

	bool IsDead() const override;

	// ----------------------------------
	// ------- Third Person Cam ---------
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
	//			Bending
	// ----------------------------------

	public:
	UPROPERTY(Replicated,VisibleDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float BendDownDegree = 0.f;
	UFUNCTION(BlueprintCallable)
	virtual void UpdateBendDownDegree();
	UFUNCTION(BlueprintCallable,Server,Unreliable)
	virtual void SetBendDownDegree_Server(float _Degree);

	// --------------------
	// ----- Weapon -----
	// --------------------
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon",meta = (AllowPrivateAccess="true"))
	class UWeaponComponent* WeaponComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon",meta = (AllowPrivateAccess="true"))
	USkeletalMeshComponent* TPWeaponComponent;

public:
	UFUNCTION(BlueprintCallable)
	class UWeaponComponent* GetWeaponComponent() const;
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetTPWeaponComponent() const;
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FPWeaponMeshComponent;

public:
	UFUNCTION(BlueprintCallable)
	class USkeletalMeshComponent* GetFPWeaponMeshComponent();
	
	// ------------------------------
	//				AI
	// ------------------------------
public:
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = "AI")
	bool bIsAI = false;


};
