#include "CPP_PGCharacter.h"
#include "Net\UnrealNetwork.h"

// Sets default values
ACPP_PGCharacter::ACPP_PGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void ACPP_PGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_PGCharacter, MoveForwardAxis);
	DOREPLIFETIME(ACPP_PGCharacter, MoveRightAxis);
}

// Called when the game starts or when spawned
void ACPP_PGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_PGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACPP_PGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

void ACPP_PGCharacter::SetMoveForwardAxis(float _Axis)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		//움직임값 동일하면 서버에 전송하지 않는다
		if (MoveForwardAxis != _Axis)
		{
			SetMoveForwardAxis_Server(_Axis);
		}
	}
	MoveForwardAxis = _Axis;
}

void ACPP_PGCharacter::SetMoveForwardAxis_Server_Implementation(float _Axis)
{
	MoveForwardAxis = _Axis;
}

void ACPP_PGCharacter::SetMoveRightAxis(float _Axis)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		//움직임값 동일하면 서버에 전송하지 않는다
		if (MoveRightAxis != _Axis)
		{
			SetMoveRightAxis_Server(_Axis);
		}
		SetMoveRightAxis_Server(_Axis);
	}
	MoveRightAxis = _Axis;
}

void ACPP_PGCharacter::SetMoveRightAxis_Server_Implementation(float _Axis)
{
	MoveRightAxis = _Axis;
}

ENetRole ACPP_PGCharacter::GetNetRole()
{
	if (HasAuthority())
	{
		return ROLE_Authority;
	}
	if (GetController() == GetWorld()->GetFirstPlayerController())
	{
		return ROLE_AutonomousProxy;
	}

	return ROLE_SimulatedProxy;
}

bool ACPP_PGCharacter::IsMyComputer()
{
	if (GetNetMode() == NM_Standalone)
	{
		return true;
	}
	else if (GetNetMode() == NM_Client)
	{
		if (GetNetRole() == ROLE_AutonomousProxy)
		{
			return true;
		}
	}
	return false;

}

