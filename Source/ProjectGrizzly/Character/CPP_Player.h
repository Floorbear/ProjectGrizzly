// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_PlayableCharacter.h"
#include "CPP_Player.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_Player : public ACPP_PlayableCharacter
{
	GENERATED_BODY()
	ACPP_Player();

protected:
	void OnRep_PlayerState() override;
	virtual void OnRep_Owner() override;


	void PossessedBy(AController* NewController) override;

	void BindASCInput();

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void BeginPlay() override;

	void Tick(float DeltaTime) override;



	// ---------------------------------------------
	//					FP Camera
	// ---------------------------------------------
private:
	FVector StandingCameraLocation;
	FVector CrouchingCameraLocation;

	void UpdateCameraCrouch(float _DeltaTime);

	float HighReady = 0.f;
public:
	
private:
	float HighReadyInterpo = 0.f;
	FVector CurrentHandsLocationInterpo = { 0,0,0 };
	UFUNCTION(BlueprintCallable)
	void HandSway(float _DeltaTime);
	void UpdateHighReady(TPair<bool,FHitResult>& _HitPair);
	void LeaningCamera(float _DeltaTime);
	FVector GetHighReadyLocation() const
	{
		return { -5.f,10.f,-25.f };
	}
	FRotator GetHighReadyRotation() const
	{
		return FRotator(0.f, -90.f, -40.f);
	}

	UFUNCTION(BlueprintCallable)
	void UpdateHighReadyInterpo(float _DeltaTime);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HandSwayXDelta = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HandSwayYDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MaxHandSwayDegree = -4.f;
	// ---------------------------------------------
	//					Crouch
	// ---------------------------------------------
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	EFaction GetFaction() const override;
	UFactionComponent* GetFactionComponent() const override;

	//--------------------------------------------------------------------------------------------------
	//										SwapWeapon
	//--------------------------------------------------------------------------------------------------
public:
	void OnCurrentWeaponSlot();
	virtual ACPP_WeaponInstance* GetCurrentWeaponInstance() override;
	virtual ACPP_WeaponInstance* GetWeaponInstanceFromSlot(EWeaponSlot _Slot) override;
	virtual void TryActivateSwapWeapon(EWeaponSlot _Slot) override;

protected:
	virtual void SwapWeapon(EWeaponSlot _Slot) override;


	//--------------------------------------------------------------------------------------------------
	//										Interaction
	//--------------------------------------------------------------------------------------------------
private:
	class IInteractable* PrevInteractableObject = nullptr;

	
	bool CanInteractWithTarget(TPair<bool, FHitResult>& HitPair) const;
	TPair<bool,FHitResult> GetFrontHitResult() const;
	
	UFUNCTION(BlueprintCallable)
	void UpdateFrontInteraction(float _DeltaTime);

	UFUNCTION()
	void TryInteract();
	
	// ToDo : °¢ Weapon ÀÇ HighReadyDistance Get
	float GetInteractionDistance() const
	{
		return 200.f;
	}

	//--------------------------------------------------------------------------------------------------
	//										UseItem
	//--------------------------------------------------------------------------------------------------
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = UseItem, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* UsableItemMeshComponent = nullptr;
public:
	UFUNCTION(BlueprintCallable,Category = UseItem)
	USkeletalMeshComponent* GetUsableItemMeshComponent() const
	{
		return UsableItemMeshComponent;
	}
	UFUNCTION(BlueprintCallable,Category = UseItem)
	void TryUseItem(ACPP_Item* _Item);
};
