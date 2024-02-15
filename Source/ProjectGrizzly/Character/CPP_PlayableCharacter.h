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

protected:
	void BeginPlay() override;

	void OnRep_PlayerState() override;

	void PossessedBy(AController* NewController) override;

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ----- GAS ------
private:
	class UAbilitySystemComponent* AbilitySystemComponent;
	class UGrizzlyAttributeSet* AttributeSet;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGrizzlyAbility>> PlayerAbilies;

	bool ASCInputBound = false;
	void BindASCInput();

	// ----- Camera -----
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




	// ----- Hands -----
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Hands", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* HandsMeshComponent;

public:
	UFUNCTION(BlueprintCallable)
	class USkeletalMeshComponent* GetHandsMeshComponent();

		// ----- Weapon ToDo : Weapon 액터로 옮길 것 -----
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FPWeaponMeshComponent;

public:
	UFUNCTION(BlueprintCallable)
	class USkeletalMeshComponent* GetFPWeaponMeshComponent();
	


};
