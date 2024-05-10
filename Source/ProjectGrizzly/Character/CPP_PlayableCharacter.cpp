
#include "CPP_PlayableCharacter.h"
#include <GameFramework\SpringArmComponent.h>
#include <Camera\CameraComponent.h>
#include "CPP_GrizzlyPS.h"
#include "Ability\GrizzlyAbility.h"
#include "AbilitySystemComponent.h"
#include "GrizzlyPC.h"
#include <Kismet/KismetMathLibrary.h>
#include <Components/CapsuleComponent.h>
#include "..\Weapon\WeaponComponent.h"
#include "Net\UnrealNetwork.h"

#include "ProjectGrizzly/ProjectGrizzly.h"
//ToDo : AI쪽으로 가야할 헤더
#include "AIModule\Classes\AIController.h"

ACPP_PlayableCharacter::ACPP_PlayableCharacter()
{

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetRelativeLocation({37.f,0,77.f});
	Camera->SetFieldOfView(70.f);
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 0.f;
	SpringArm->SetupAttachment(Camera);


	GetCapsuleComponent()->SetCapsuleRadius(34.0f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.f);

	HandsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMeshComponent"));
	HandsMeshComponent->SetRelativeLocation(IdleHandLocation);
	HandsMeshComponent->SetRelativeRotation(FRotator(0, -90.f,0).Quaternion());
	//ToDo : 복장에 따른 손 메쉬 변화
	//static auto HandsMesh = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Game/ProjectGrizzly/Character/FirstPersonHand/HandMesh/Merc_Metro/hand_merc_metro.hand_merc_metro"));
	//HandsMeshComponent->SetSkeletalMesh(HandsMesh.Object);
	//static auto HandsMeshClass = ConstructorHelpers::FClassFinder<UAnimInstance>(TEXT("/Game/ProjectGrizzly/Character/FirstPersonHand/ABP_Hand.ABP_Hand_C"));
	HandsMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//HandsMeshComponent->SetAnimInstanceClass(HandsMeshClass.Class);
	HandsMeshComponent->SetBoundsScale(1.5f);
	HandsMeshComponent->SetupAttachment(SpringArm);


	FPWeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPWeaponMeshComponent"));
	FPWeaponMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	FPWeaponMeshComponent->SetBoundsScale(1.5f);
	FPWeaponMeshComponent->SetupAttachment(HandsMeshComponent);

	GetMesh()->SetRelativeLocation({0,0,-90.f});
	GetMesh()->SetRelativeRotation(FRotator(0, 270.f, 0).Quaternion());
	//ToDo : 복장에 따른 캐릭터 메쉬 변화
	//static auto CharacterMesh = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Game/ProjectGrizzly/Character/Model/Merc/Merc_2a/SKM_Merc_2a.SKM_Merc_2a"));
	//GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//static auto CharacterMeshClass = ConstructorHelpers::FClassFinder<UAnimInstance>(TEXT("/Game/ProjectGrizzly/Character/ABP_Stalker.ABP_Stalker_C"));
	//GetMesh()->SetAnimInstanceClass(CharacterMeshClass.Class);
	GetMesh()->SetIsReplicated(true);



	TPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArm"));
	TPSpringArm->SetupAttachment(RootComponent);
	TPSpringArm->TargetArmLength = 100.f;

	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	TPCamera->SetupAttachment(TPSpringArm);

	TPWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TPWeaponComponent"));
	TPWeaponComponent->SetupAttachment(GetMesh(), TEXT("WeaponRight"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetIsReplicated(true);
	RemoteViewPitch = true;
}
void ACPP_PlayableCharacter::UpdateBendDownDegree()
{
	if (IsMyComputer())
	{
		float CurrentDegree = GetControlRotation().Pitch;
		if (FMath::Abs(CurrentDegree - BendDownDegree) >= 0.01)
		{
			SetBendDownDegree_Server(CurrentDegree);
		}
		BendDownDegree = CurrentDegree;
	}
}

void ACPP_PlayableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_PlayableCharacter, BendDownDegree);
	DOREPLIFETIME(ACPP_PlayableCharacter, bIsAI);
}
void ACPP_PlayableCharacter::SetBendDownDegree_Server_Implementation(float _Degree)
{
	BendDownDegree = _Degree;
}
class UWeaponComponent* ACPP_PlayableCharacter::GetWeaponComponent() const
{
	check(WeaponComponent != NULL)
		return WeaponComponent;
}

USkeletalMeshComponent* ACPP_PlayableCharacter::GetTPWeaponComponent() const
{
	return TPWeaponComponent;
}

void ACPP_PlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateBendDownDegree();
	//UpdateADS , Only Update My computer
	if(IsMyComputer())
	{
		UpdateADS(DeltaTime);
		if (IsDead())
		{
			UpdateDeathCam(DeltaTime);
		}
	}
}

void ACPP_PlayableCharacter::UpdateADS(float DeltaTime)
{
	USkeletalMeshComponent* Hands = GetHandsMeshComponent();
	FVector CurrentLocation = Hands->GetRelativeLocation();
	FVector InterpoValue = { 0,0,0 };
	float OriginDistance = FVector::Distance({ 0,0,0 }, IdleHandLocation);

	if (bADS)
	{
		InterpoValue = FMath::VInterpTo(CurrentLocation, { 0,0,0 }, DeltaTime, GetADSSpeed());
	}
	else
	{
		InterpoValue = FMath::VInterpTo(CurrentLocation, IdleHandLocation, DeltaTime, GetADSSpeed());
		// Except : 재장전 등 이유로 캔슬될 때 즉시 포지션 이동

		//줌을 거의 다 풀면 어빌리티 비활성화
		{
			//float Distance = FVector::Distance(InterpoValue, IdleHandLocation);
			//float MarginOfError = 0.02;
			//if (Distance < MarginOfError)
			//{
			//	Hands->SetRelativeLocation(IdleHandLocation);
			//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			//}
		}

	}
	float Distance = FVector::Distance(InterpoValue, IdleHandLocation);
	ADSFactor = Distance / OriginDistance;
	//float MarginOfError = 0.002;
	//if (ADSFactor + MarginOfError > 0 && 1.0f - MarginOfError > ADSFactor)
	//{

	//}
	Hands->SetRelativeLocation(InterpoValue);
}



class UCameraComponent* ACPP_PlayableCharacter::GetCamera() const
{
	return Camera;
}

void ACPP_PlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	TPCamera->SetActive(false);
}




class USkeletalMeshComponent* ACPP_PlayableCharacter::GetHandsMeshComponent()
{
	return HandsMeshComponent;
}

float ACPP_PlayableCharacter::GetADSSpeed()
{
	return 6.0f;
}

void ACPP_PlayableCharacter::SetADS(bool _NewBool)
{
	bADS = _NewBool;
}

bool ACPP_PlayableCharacter::GetADS() const
{
	return bADS;
}

float ACPP_PlayableCharacter::GetADSFactor() const
{
	return ADSFactor;
}

void ACPP_PlayableCharacter::Die()
{
	Super::Die();
	//Die 이펙트 적용
	FGameplayEffectContextHandle Context = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DeadGameplayEffect,1,Context);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	Die_Multicast();
}

void ACPP_PlayableCharacter::Die_Multicast_Implementation()
{
	//Autonomus Proxy or Single Mode
	if (IsMyComputer() && !bIsAI)
	{
		//사망시 3인칭 캐릭터 제대로 보이게 & 1인칭 메쉬 끄기
		DeadLocation = GetMesh()->GetSocketLocation(TEXT("Pelvis"));
		HandsMeshComponent->SetVisibility(false);
		FPWeaponMeshComponent->SetVisibility(false);
		GetTPWeaponComponent()->SetVisibility(true);
		GetMesh()->UnHideBoneByName(TEXT("bip01_spine"));
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//카메라 연출
		Camera->SetActive(false);
		TPCamera->SetActive(true);
		float xPos = FMath::RandRange(-100.f, 100.f);
		float yPos = FMath::RandRange(-100.f, 100.f);
		float zPos = FMath::RandRange(-20.f,100.f);
		TPSpringArm->SetRelativeLocation({xPos,yPos,zPos},true);
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TPSpringArm->GetComponentLocation(),DeadLocation);
		TPSpringArm->SetWorldRotation(LookAtRotation);

		//UI
		AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetController());
		PC->OnDeadNotificationUI();
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);

}

bool ACPP_PlayableCharacter::IsDead() const
{
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	if (!IsValid(PS))
	{
		return false;
	}
	return !PS->IsAlive();
}

void ACPP_PlayableCharacter::UpdateDeathCam(float _DeltaTime)
{
	DeathCamSpeed = FMath::FInterpTo(DeathCamSpeed,0, _DeltaTime,5.0);
	FVector LookAtDir = TPSpringArm->GetComponentLocation() - DeadLocation;
	LookAtDir.Normalize();
	TPSpringArm->AddWorldOffset(LookAtDir * DeathCamSpeed,true);
}

class USkeletalMeshComponent* ACPP_PlayableCharacter::GetFPWeaponMeshComponent()
{
	return FPWeaponMeshComponent;
}
