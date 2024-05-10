// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_AI_PlayableCharacter_PC.h"
#include "Perception/AISense_Sight.h"
#include "CPP_PGCharacter.h"
#include "AIModule\Classes\BehaviorTree\BlackboardComponent.h"
#include "Kismet\GameplayStatics.h"

ACPP_AI_PlayableCharacter_PC::ACPP_AI_PlayableCharacter_PC()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void ACPP_AI_PlayableCharacter_PC::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&ACPP_AI_PlayableCharacter_PC::OnTargetPerceptionUpdated);
	LostTargetActorTime = MaxLostTargetActorTime;
}

ACharacter* ACPP_AI_PlayableCharacter_PC::GetTargetActor()
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
}

void ACPP_AI_PlayableCharacter_PC::UpdateTargetLocation()
{
	
}

void ACPP_AI_PlayableCharacter_PC::OnTargetPerceptionUpdated(AActor* _Actor, FAIStimulus _Stimulus)
{
	UpdateSightPerception(_Actor,_Stimulus);
}

void ACPP_AI_PlayableCharacter_PC::UpdateSightPerception(AActor* _Actor, FAIStimulus _Stimulus)
{

	if (UAISense_Sight::StaticClass() != UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), _Stimulus))
	{
		return;
	}

	if (Cast<ACPP_PGCharacter>(_Actor) == NULL)
	{
		return;
	}

	bool bSuccessfullySensed =  _Stimulus.WasSuccessfullySensed();
	if (bSuccessfullySensed)
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("LostsTargetActor"), false);
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"),GetTargetActor());
		GetWorldTimerManager().ClearTimer(LostTargetActor_TimerHandle);
	}
	else
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), NULL);
		LostTargetActorTime = MaxLostTargetActorTime;

		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([this]()
		{
			LostTargetActorTime = FMath::Clamp(LostTargetActorTime - 0.1f,0,99999.f);
			if(!(LostTargetActorTime > 0))
			{
				GetBlackboardComponent()->SetValueAsBool(TEXT("LostsTargetActor"),true);
				GetWorldTimerManager().ClearTimer(LostTargetActor_TimerHandle);
			}
		});
		GetWorldTimerManager().SetTimer(LostTargetActor_TimerHandle,TimerCallback,0.1,true);
	}
}

float ACPP_AI_PlayableCharacter_PC::GetBulletThreat()
{
	return GetBlackboardComponent()->GetValueAsFloat(TEXT("BulletThreat"));
}

void ACPP_AI_PlayableCharacter_PC::SetBulletThreat(float _Value)
{
	float Value = FMath::Clamp(_Value,0,1.f);
	GetBlackboardComponent()->SetValueAsFloat(TEXT("BulletThreat"), Value);
}

