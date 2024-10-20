// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GrizzlyGameState.h"
#include "ProjectGrizzly/ProjectGrizzly.h"

//게임모드가 StartPlay를 지시할때 게임스테이트에 명령을 내려서 게임스테이트가 월드에 있는 모든액터에게 BeginPlay를 호출하도록 명령을 내린다
//단 서버에서만 실행되는 로직이기 때문에 클라이언트에서 호출되지 않는다
void ACPP_GrizzlyGameState::HandleBeginPlay()
{
	Super::HandleBeginPlay();

}

// HandleBeginPlay와 동일한 로직을 수행하지만
// bReplicatedHasBegunPlay 프로퍼티가 변경되 클라이언트로 복제됐을 때 '클라이언트'에서만 호출된다
void ACPP_GrizzlyGameState::OnRep_ReplicatedHasBegunPlay()
{
	Super::OnRep_ReplicatedHasBegunPlay();

}
