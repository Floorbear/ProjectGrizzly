// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionComponent.h"
#include "Net\UnrealNetwork.h"
#include "CPP_PGCharacter.h"

// Sets default values for this component's properties
UFactionComponent::UFactionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	//ToDo : DT to GameInstance
	static auto DT = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Character/Faction/DT_FactionRelationship.DT_FactionRelationship"));
	DT_FactionRelationship = DT.Object;
}


// Called when the game starts
void UFactionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!IsSetFaction())
		UE_LOG(LogTemp, Warning, TEXT("%s's faction is not set"), *GetName());
}                                     

void UFactionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFactionComponent, MyFaction);
}


// Called every frame
void UFactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFactionComponent::SetFaction_Implementation(EFaction _Faction)
{
	checkf(_Faction != EFaction::NonSet,TEXT("Faction::None should not used"));
	MyFaction = _Faction;
}
    
bool UFactionComponent::IsSetFaction() const
{
	if (MyFaction == EFaction::NonSet)
		return false;
	return true;
}

bool UFactionComponent::IsHostile(ACPP_PGCharacter* _TargetActor) const
{
	EFaction Faction = _TargetActor->GetFaction();
	const UEnum* FactionRelationshipEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFaction"), true);
	ensure(FactionRelationshipEnum);
	FName EnumToName(FactionRelationshipEnum->GetDisplayNameTextByValue((int64)Faction).ToString());

	FFactionData* FactionData = GetFactionRelationShip_DT()->FindRow<FFactionData>(EnumToName, FString(""));
	if(FactionData->HostileFaction.Contains(MyFaction))
		return true;
	return false;
}

UDataTable* UFactionComponent::GetFactionRelationShip_DT() const
{
	ensure(IsValid(DT_FactionRelationship)); //DT_FactionRelationship가 로드되지 않음
	return DT_FactionRelationship;
}

