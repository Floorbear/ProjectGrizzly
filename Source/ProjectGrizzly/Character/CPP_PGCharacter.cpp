#include "CPP_PGCharacter.h"
#include "Net\UnrealNetwork.h"
#include "CPP_GrizzlyPS.h"
#include <GameFramework\CharacterMovementComponent.h>
#include "AIModule\Classes\AIController.h"

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
	}
	MoveRightAxis = _Axis;
}

void ACPP_PGCharacter::SetMoveRightAxis_Server_Implementation(float _Axis)
{
	MoveRightAxis = _Axis;
}


void ACPP_PGCharacter::SetSpeed_Multicast_Implementation(float _Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = _Speed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = _Speed * CrouchRate;
}

void ACPP_PGCharacter::SetSpeed_Server_Implementation(float _Speed)
{
	SetSpeed_Multicast(_Speed);
}

void ACPP_PGCharacter::SetSpeed(float _Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = _Speed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = _Speed * CrouchRate;
	if (HasAuthority())
	{
		SetSpeed_Multicast(_Speed);
	}
	else
	{	
		SetSpeed_Server(_Speed);
	}
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
	//AAIController* AIController = Cast<AAIController>(GetController());
	//if (AIController != NULL)
	//{
	//	return false;
	//}
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

UAbilitySystemComponent* ACPP_PGCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}



void ACPP_PGCharacter::Die()
{

}

bool ACPP_PGCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

	FHitResult HitResult;

	//우선 Torso가 보이면 사격
	{
		FVector socketLocation = GetMesh()->GetSocketLocation(TEXT("Torso"));
		const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, socketLocation
			, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)) // << Changed this line
			, FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

		NumberOfLoSChecksPerformed++;

		if (bHit == false || (IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsOwnedBy(this)))
		{
			OutSeenLocation = GetActorLocation();
			OutSightStrength = 1;

			return true;
		}
	}
	//Torso가 보이지 않으면 머리 테스트
	{
		FVector socketLocation = GetMesh()->GetSocketLocation(TEXT("Head"));
		const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, socketLocation
			, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)) // << Changed this line
			, FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

		NumberOfLoSChecksPerformed++;

		if (bHit == false || (IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsOwnedBy(this)))
		{
			OutSeenLocation = GetActorLocation();
			OutSightStrength = 1;

			return true;
		}
	}

	//Torso가 보이지 않으면 나머지 부위(팔,다리, 머리) 테스트
	auto sockets = GetMesh()->GetAllSocketNames();

	for (int i = 0; i < sockets.Num(); i++)
	{
		FVector socketLocation = GetMesh()->GetSocketLocation(sockets[i]);

		const bool bHitSocket = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, socketLocation
			, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)) // << Changed this line
			, FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

		NumberOfLoSChecksPerformed++;

		if (bHitSocket == false || (IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsOwnedBy(this))) {
			OutSeenLocation = socketLocation;
			OutSightStrength = 1;

			return true;
		}
	}



	OutSightStrength = 0;
	return false;



	//static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));
	//FHitResult HitResult;
	////Get the names of all the sockets in a mesh. Not used in this example.
	//auto sockets = GetMesh()->GetAllSocketNames();
	////Here we consider the Head as the source
	//FVector socketLocation = GetMesh()->GetSocketLocation("Head");

	////Check if the neck socket is visible
	//const bool bHitSocket = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, socketLocation, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)), FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));
	//NumberOfLoSChecksPerformed++;
	//if (bHitSocket == false || (IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsOwnedBy(this)))
	//{
	//	OutSeenLocation = socketLocation;
	//	OutSightStrength = 1;
	//	UE_LOG(LogTemp, Warning, TEXT("TRUE"));
	//	return true;
	//}


	//UE_LOG(LogTemp, Warning, TEXT("FALSE"));
	//OutSightStrength = 0;
	//return false;
}

