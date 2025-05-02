// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Ability\GrizzlyAttributeSet.h"
#include "Perception/AISightTargetInterface.h"
#include "FactionComponent.h"
#include "CPP_PGCharacter.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_PGCharacter : public ACharacter, public IAbilitySystemInterface, public IAISightTargetInterface
{
	GENERATED_BODY()
	friend class UCPP_A_PGCharacter;

public:
	// Sets default values for this character's properties
	ACPP_PGCharacter();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ----------------------------------
	//			Movement
	// ----------------------------------

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MoveForwardAxis = 0;
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MoveRightAxis = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchRate = 0.5f;
	UFUNCTION(BlueprintCallable)
	void SetMoveForwardAxis(float _Axis);
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SetMoveForwardAxis_Server(float _Axis);

	UFUNCTION(BlueprintCallable)
	void SetMoveRightAxis(float _Axis);
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SetMoveRightAxis_Server(float _Axis);



	UFUNCTION(BlueprintCallable, Reliable, NetMulticast)
	void SetSpeed_Multicast(float _Speed);
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void SetSpeed_Server(float _Speed);
	void SetSpeed(float _Speed);

	// ---------------
	// ----- GAS -----
	// ---------------

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UGrizzlyAttributeSet> AttributeSet;

	// ------ Network -----
	UFUNCTION(BlueprintCallable)
	ENetRole GetNetRole();

	UFUNCTION(BlueprintCallable)
	bool IsMyComputer(); // 싱글, 멀티 둘 다 고려
public:
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	// ---------------
	// ----- Dead ----
	// ---------------
public:
	virtual void Die();

	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() const { return false; };

	// ---------------------------
	//			Faction
	// ---------------------------
public:
	UFUNCTION(BlueprintCallable)
	virtual EFaction GetFaction() const PURE_VIRTUAL(ACPP_PGCharacter::GetFaction, return EFaction::NonSet;);

public:
	UFUNCTION(BlueprintCallable)
	virtual UFactionComponent* GetFactionComponent() const PURE_VIRTUAL(ACPP_PGCharacter::GetFactionComponent, return NULL;);

	//--------------------------------------------------------------------------------------------------
	//										Sound
	//--------------------------------------------------------------------------------------------------
private:


	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* CharacterAudioComponent;

	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* GunAudioComponent;
	//두 함수 전부 GAS에서 사용함
	UFUNCTION(NetMulticast,Unreliable)
	void PlaySoundAtLocation_Prediction(USoundBase* _Sound,FVector _Location,ACPP_PGCharacter* _Instigator);
	UFUNCTION(NetMulticast,Unreliable)
	void PlaySoundAtComponent_Prediction(UAudioComponent* _Component,USoundBase* _Sound,ACPP_PGCharacter* _Instigator);
public:
	UFUNCTION(BlueprintCallable)
	UAudioComponent* GetWeaponAudioComponent() const
	{
		return GunAudioComponent;
	}

	UFUNCTION(BlueprintCallable)
	UAudioComponent* GetCharacterAudioComponent() const
	{
		return CharacterAudioComponent;
	}

	//GAS에서만 정상적으로 작동함
	//즉 서버, 로컬 클라이언트 두개의 실행흐름을 가질때 작동함
	UFUNCTION(BlueprintCallable)
	void PlaySoundAtLocation(USoundBase* _Sound,FVector _Location);

	UFUNCTION(BlueprintCallable)
	void PlaySoundAtComponent(UAudioComponent* _Component,USoundBase* _Sound);

	//---------------------------------------------------
	//						Perception
	//---------------------------------------------------

/**
 * The method needs to check whether the implementer is visible from given observer's location.
 * @param ObserverLocation	The location of the observer
 * @param OutSeenLocation	The first visible target location
 * @param OutSightStrengh	The sight strength for how well the target is seen
 * @param IgnoreActor		The actor to ignore when doing test
 * @param bWasVisible		If available, it is the previous visibility state
 * @param UserData			If available, it is a data passed between visibility tests for the users to store whatever they want
 * @return	True if visible from the observer's location
 */
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr, const bool* bWasVisible = nullptr, int32* UserData = nullptr) const override;
};
