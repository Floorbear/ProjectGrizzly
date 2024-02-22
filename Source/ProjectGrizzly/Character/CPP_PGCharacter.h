// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Ability\GrizzlyAttributeSet.h"
#include "CPP_PGCharacter.generated.h"

UCLASS()
class PROJECTGRIZZLY_API ACPP_PGCharacter : public ACharacter, public IAbilitySystemInterface
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

		// ----- Movement -----
	UPROPERTY(Replicated,VisibleDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float MoveForwardAxis = 0;
	UPROPERTY(Replicated,VisibleDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float MoveRightAxis = 0;

	UFUNCTION(BlueprintCallable)
	void SetMoveForwardAxis(float _Axis);
	UFUNCTION(BlueprintCallable,Server,Unreliable)
	void SetMoveForwardAxis_Server(float _Axis);

	UFUNCTION(BlueprintCallable)
	void SetMoveRightAxis(float _Axis);
	UFUNCTION(BlueprintCallable,Server,Unreliable)
	void SetMoveRightAxis_Server(float _Axis);

	UPROPERTY(Replicated,VisibleDefaultsOnly,BlueprintReadWrite,Category = "Movement")
	float BendDownDegree = 0.f;
	//현재 컨트롤러의 피치 각도로 업데이트 합니다 , 아직 PGCharacter 에 있어야 할지, PlayableCharacter에 있어야 할 지 모르겠어서 여기다 둠
	UFUNCTION(BlueprintCallable)
	virtual void UpdateBendDownDegree();
	UFUNCTION(BlueprintCallable,Server,Unreliable)
	virtual void SetBendDownDegree_Server(float _Degree);

	// ----- GAS
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UGrizzlyAttributeSet> AttributeSet;

	// ------ Network -----
	UFUNCTION(BlueprintCallable)
	ENetRole GetNetRole();

	UFUNCTION(BlueprintCallable)
	bool IsMyComputer(); // 싱글, 멀티 둘 다 고려
	public:
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// ----- TPWeapon -----
private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon",meta = (AllowPrivateAccess="true"))
	USkeletalMeshComponent* TPWeaponComponent;

public:
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetTPWeaponComponent() const;

};
