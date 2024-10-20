// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine/DataTable.h>
#include "FactionComponent.generated.h"

UENUM(BlueprintType)
enum class EFaction : uint8
{
	NonSet				UMETA(DisplayName = "NonSet"), //팩션에 무조건 소속해 있어야함 > 따라서  NonSet일 경우는 팩션 설정이 안된 '잘못된' 상태
	Mercenary			UMETA(DisplayName = "Mercenary"),
	Bandit				UMETA(DisplayName = "Bandit")
};

USTRUCT(BlueprintType)
struct FFactionData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FactionID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFaction Faction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EFaction> HostileFaction;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIZZLY_API UFactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFactionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void SetFaction(EFaction _Faction);
	UFUNCTION(BlueprintCallable)
	EFaction GetFaction() const
	{
		return MyFaction;
	}
	UFUNCTION(BlueprintCallable)
	bool IsSetFaction() const;

	UFUNCTION(BlueprintCallable)
	bool IsHostile(class ACPP_PGCharacter* _TargetActor) const;

private:
	UDataTable* GetFactionRelationShip_DT() const;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Faction, meta = (AllowPrivateAccess = "true"), Replicated)
	EFaction MyFaction = EFaction::NonSet;

	UPROPERTY()
	UDataTable* DT_FactionRelationship;
};
