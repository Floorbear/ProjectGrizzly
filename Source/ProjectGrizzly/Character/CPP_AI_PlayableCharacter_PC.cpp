// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_AI_PlayableCharacter_PC.h"
#include "Perception/AISense_Sight.h"
#include "CPP_PGCharacter.h"
#include "AIModule\Classes\BehaviorTree\BlackboardComponent.h"
#include "CPP_AIPlayableCharacter.h"
#include "PlayerInventoryComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"

ACPP_AI_PlayableCharacter_PC::ACPP_AI_PlayableCharacter_PC()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	static auto DT = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Character/AI/Patrol/DT_PatrolRoute.DT_PatrolRoute"));
	PatrolDT = DT.Object;
	
}

void ACPP_AI_PlayableCharacter_PC::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//Perception 관련 초기화
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACPP_AI_PlayableCharacter_PC::OnTargetPerceptionUpdated);
	LostTargetActorTime = MaxLostTargetActorTime;




	FOnThreatened.AddDynamic(this, &ACPP_AI_PlayableCharacter_PC::OnThreatened);
}

void ACPP_AI_PlayableCharacter_PC::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, false);

	//Smooth and change the pawn rotation
	if (bUpdatePawn)
	{
		//Get pawn
		APawn* const MyPawn = GetPawn();
		//Get Pawn current rotation
		const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

		//Calculate smoothed rotation
		SmoothTargetRotation = UKismetMathLibrary::RInterpTo_Constant(MyPawn->GetActorRotation(), ControlRotation, DeltaTime, SmoothTargetRotationSpeed);
		//Check if we need to change
		if (CurrentPawnRotation.Equals(SmoothTargetRotation, 1e-3f) == false)
		{
			//Change rotation using the Smooth Target Rotation
			MyPawn->FaceRotation(SmoothTargetRotation, DeltaTime);
		}
	}
}

ACharacter* ACPP_AI_PlayableCharacter_PC::GetTargetActor()
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ACPP_AI_PlayableCharacter_PC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTargetLocation();
}

void ACPP_AI_PlayableCharacter_PC::BeginPlay()
{
	Super::BeginPlay();
	InitPatrol();

}

void ACPP_AI_PlayableCharacter_PC::UpdateTargetLocation()
{
	//타겟 엑터가 보이지 않으면 위치 업데이트 X 
	if (GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")) == NULL)
	{
		return;
	}

	//FVector TargetLocation = Cast<ACPP_PGCharacter>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")))->GetActorLocation();
	GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), PerceptionComponent->GetActorLocation(*Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")))));
}

bool ACPP_AI_PlayableCharacter_PC::TraceToTargetLocation() const
{
	//TargetLocation값이 존재하지 않으면 장애물이 있다고 인식함
	//이러는 이유는 이 상황에 false를 하면 벽을 향해 총을 쏘는 상황이 있을 수 있어서
	if (!GetBlackboardComponent()->IsVectorValueSet(TEXT("TargetLocation")))
	{
		return true;
	}
	float Height = 30.f;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParam = FCollisionQueryParams::DefaultQueryParam;
	FVector StartPos = GetPawn()->GetActorLocation();
	StartPos.Z += Height;
	FVector EndPos = GetBlackboardComponent()->GetValueAsVector(TEXT("TargetLocation"));
	EndPos.Z += Height;
	CollisionParam.AddIgnoredActor(GetPawn()->GetUniqueID());
	return GetWorld()->LineTraceSingleByChannel
	(HitResult, StartPos, EndPos, ECC_Visibility, CollisionParam);
}

void ACPP_AI_PlayableCharacter_PC::UpdateSense()
{
	ACPP_PGCharacter* SourceActor = Cast<ACPP_AIPlayableCharacter>(GetPawn());
	ACPP_PGCharacter* TargetActor = CalculateTargetActor();

	//주변에 살아있는 적이 없다면 탐색 모드로 전환하게 설정
	if (PerceptionTable.IsEmpty())
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("LostsTargetActor"), true);
	}

	//TargetActor = IsValid   Mean : WasSuccessfullySensed가 True인 액터 중 거리가 가장 가까운 액터
	if (IsValid(TargetActor))
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("LostsTargetActor"), false);
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), TargetActor);
		GetWorldTimerManager().ClearTimer(LostTargetActor_TimerHandle);
	}
	else //시야에 보이는 적이 존재하지 않음
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), NULL);

		//가장 가까운 적의 마지막 위치를 TargetLocation으로 기록
		if (PerceptionTable.IsEmpty())
		{
			GetBlackboardComponent()->ClearValue(TEXT("TargetLocation"));
		}
		else
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), PerceptionTable.begin().Value().StimulusLocation);
		}

		//살아있는 적이 있으나 일정 시간 동안 적이 보이지 않을경우 탐색모드로 전환
		LostTargetActorTime = MaxLostTargetActorTime;
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([this]()
			{
				LostTargetActorTime = FMath::Clamp(LostTargetActorTime - 0.1f, 0, 99999.f);
				if (!(LostTargetActorTime > 0))
				{
					GetBlackboardComponent()->SetValueAsBool(TEXT("LostsTargetActor"), true);
					GetWorldTimerManager().ClearTimer(LostTargetActor_TimerHandle);
				}
			});
		if (!LostTargetActor_TimerHandle.IsValid())
		{
			GetWorldTimerManager().SetTimer(LostTargetActor_TimerHandle, TimerCallback, 0.1, true);
		}
	}
}

FVector ACPP_AI_PlayableCharacter_PC::GetFocalPointOnActor(const AActor* Actor) const
{
	auto PGActor = Cast<ACPP_PGCharacter>(Actor);
	if (PGActor)
	{
		bool WasVisible = false;
		return Actor != nullptr ? PerceptionComponent->GetActorLocation(*Actor) : FAISystem::InvalidLocation;

	}

	return Actor != nullptr ? Actor->GetActorLocation() : FAISystem::InvalidLocation;

}

void ACPP_AI_PlayableCharacter_PC::TransmitTargetInformation(AActor* _Actor, FAIStimulus _Stimulus)
{
	// 호출한 AI를 중심으로 구체 범위 안의 아군에게 자신이 발견한 적의 정보를 넘겨준다
	ACPP_PGCharacter* Target = Cast<ACPP_PGCharacter>(_Actor);
	ACPP_AIPlayableCharacter* Source = Cast<ACPP_AIPlayableCharacter>(GetPawn());
	if (!Target)
	{
		return;
	}
	FVector SpherePos = GetPawn()->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> PawnType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(PawnType);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetPawn());
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SpherePos, 8000, ObjectTypes, ACPP_AIPlayableCharacter::StaticClass(), IgnoreActors, OutActors);
	//아군이면 정보를 넘겨준다
	for (auto Actor : OutActors)
	{
		ACPP_AIPlayableCharacter* FriendlyActor = Cast<ACPP_AIPlayableCharacter>(Actor);
		if (!FriendlyActor)
		{
			continue;
		}
		if (Source->GetFaction() != FriendlyActor->GetFaction())
		{
			continue;
		}
		ACPP_AI_PlayableCharacter_PC* FriendlyActorController = Cast<ACPP_AI_PlayableCharacter_PC>(FriendlyActor->GetAIController());
		if (!FriendlyActorController)
			continue;
		FriendlyActorController->ReceiveTargetInformation(_Actor, _Stimulus);
	}
}

void ACPP_AI_PlayableCharacter_PC::ReceiveTargetInformation(AActor* _Actor, FAIStimulus _Stimulus)
{
	ACPP_PGCharacter* KeyActor = Cast<ACPP_PGCharacter>(_Actor);
	if (!KeyActor)
		return;

	if (PerceptionTable.Contains(KeyActor))
	{
		FAIStimulus* Value =  PerceptionTable.Find(KeyActor);
		Value->StimulusLocation = _Stimulus.StimulusLocation;
	}
	else
	{
		_Stimulus.MarkNoLongerSensed();
		PerceptionTable.Add(KeyActor, _Stimulus);
	}
	UpdateSense();

}

void ACPP_AI_PlayableCharacter_PC::OnTargetPerceptionUpdated(AActor* _Actor, FAIStimulus _Stimulus)
{
	UpdateSightPerception(_Actor, _Stimulus);
}

void ACPP_AI_PlayableCharacter_PC::UpdateSightPerception(AActor* _Actor, FAIStimulus _Stimulus)
{

	if (UAISense_Sight::StaticClass() != UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), _Stimulus))
	{
		return;
	}
	ACPP_PGCharacter* TargetActor = Cast<ACPP_PGCharacter>(_Actor);
	ACPP_PGCharacter* SourceActor = Cast<ACPP_AIPlayableCharacter>(GetPawn());
	if (TargetActor == NULL)
	{
		return;
	}
	if (TargetActor->GetFaction() == EFaction::NonSet)
	{
		return;
	}
	if (!SourceActor->GetFactionComponent()->IsHostile(TargetActor))
	{
		return;
	}
	bool bSuccessfullySensed = _Stimulus.WasSuccessfullySensed();
	//사망하면 테이블에서 제외
	if (TargetActor->IsDead())
	{
		if (PerceptionTable.Contains(TargetActor))
		{
			PerceptionTable.Remove(TargetActor);
		}
	}
	else
	{
		PerceptionTable.FindOrAdd(TargetActor) = _Stimulus;
		TransmitTargetInformation(TargetActor, _Stimulus);
		//BP_HandOverTargetInformation(TargetActor, _Stimulus);
	}
	UpdateSense();

}

ACPP_PGCharacter* ACPP_AI_PlayableCharacter_PC::CalculateTargetActor()
{
	//this와 거리 기준으로 정렬
	PerceptionTable.ValueSort([this](const FAIStimulus& _Left, const FAIStimulus& _Right)
		{
			float LeftDistance = FVector::Dist(_Left.StimulusLocation, GetPawn()->GetActorLocation());
			float RightDistance = FVector::Dist(_Right.StimulusLocation, GetPawn()->GetActorLocation());
			return LeftDistance < RightDistance;
		});
	// 거리가 가까운 적부터 엄폐물을 끼고 있지 않으면 리턴
	for (auto& i : PerceptionTable)
	{
		if (i.Value.WasSuccessfullySensed())
		{
			return i.Key;
		}
	}
	return NULL;
}

float ACPP_AI_PlayableCharacter_PC::GetBulletThreat()
{
	return GetBlackboardComponent()->GetValueAsFloat(TEXT("BulletThreat"));
}

void ACPP_AI_PlayableCharacter_PC::ApplyThreat(FThreatenResult& _ThreatenResult)
{
	FOnThreatened.Broadcast(_ThreatenResult);
}

void ACPP_AI_PlayableCharacter_PC::SetBulletThreat(float _Value)
{
	float Value = FMath::Clamp(_Value, 0, 1.f);
	GetBlackboardComponent()->SetValueAsFloat(TEXT("BulletThreat"), Value);
}

void ACPP_AI_PlayableCharacter_PC::OnThreatened(FThreatenResult& _ThreatenResult)
{
	ACPP_AIPlayableCharacter* ThisCharacter = Cast<ACPP_AIPlayableCharacter>(GetPawn());
	bool IsHostile = ThisCharacter->GetFactionComponent()->IsHostile(_ThreatenResult.Causer);
	if (IsHostile)
	{
		SetBulletThreat(_ThreatenResult.ThreatValue + GetBulletThreat());
		//ToDo : TargetLocation을 바로 업데이트하는게 아니라 타겟 액터 업데이트 함수 호출하게 변경하기
		GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), _ThreatenResult.Investigator->GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("LostsTargetActor"), false);
	}

}

void ACPP_AI_PlayableCharacter_PC::SetPatrolLocation(FVector& _Location)
{
	GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolLocation"), _Location);
}

void ACPP_AI_PlayableCharacter_PC::InitPatrol()
{
	//RouteName이 Default면 현재 위치를 로케이션으로 지정
	FVector TargetLocation = FVector::Zero();
	if(RouteName.Compare(TEXT("Default")) != 0)
	{
		PatrolRoute = PatrolDT->FindRow<FPatrolRoute>(RouteName, FString(""))->Route;
		TargetLocation = PatrolRoute[0].Get()->GetActorLocation();
	}

	GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolLocation"), TargetLocation);
	GetBlackboardComponent()->SetValueAsBool(TEXT("LostsTargetActor"), true);
}


