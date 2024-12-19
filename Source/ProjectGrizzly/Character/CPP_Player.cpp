// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Player.h"
#include <GameFramework\SpringArmComponent.h>
#include <Camera\CameraComponent.h>
#include "CPP_GrizzlyPS.h"
#include "Ability\GrizzlyAbility.h"
#include "AbilitySystemComponent.h"
#include "GrizzlyPC.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/CapsuleComponent.h>
#include "..\Weapon\WeaponComponent.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Net\UnrealNetwork.h"

#include "ProjectGrizzly/ProjectGrizzly.h"

//Client�� ȣ��
void ACPP_Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	if (!PS)
	{
		return;
	}
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();
	BindASCInput();
	SetSpeed(AttributeSet->GetSpeed());
}

// ������ Owner ���� ����Ǽ� Ŭ���̾�Ʈ�� ������ �� ȣ��Ǵ� �̺�Ʈ�Լ�
// ���������� ȣ��ȴ�
void ACPP_Player::OnRep_Owner()
{
	Grizzly_LOG( TEXT("Begin"));
	Super::OnRep_Owner();
	AActor* ActorOwner = GetOwner();
	if (ActorOwner)
	{
		Grizzly_LOG( TEXT("Owner : %s"), *ActorOwner->GetName());
	}
	else
	{
		Grizzly_LOG( TEXT("No Owner"));
	}
	Grizzly_LOG( TEXT("End"));

}


//Only ���� ȣ��
void ACPP_Player::PossessedBy(AController* NewController)
{
	Grizzly_LOG(TEXT("Begin"));
	AActor* ActorOwner = GetOwner();
	if (ActorOwner)
	{
		Grizzly_LOG( TEXT("Owner : %s"),*ActorOwner->GetName());
	}
	else
	{
		Grizzly_LOG( TEXT("No Owner"));
	}
	Super::PossessedBy(NewController);
	ActorOwner = GetOwner();
	if (ActorOwner)
	{
		Grizzly_LOG(  TEXT("Owner : %s"), *ActorOwner->GetName());
	}
	else
	{
		Grizzly_LOG(  TEXT("No Owner"));
	}
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	if (!PS)
	{
		return;
	}

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();

	//(������ �� ���) ���� ����Ʈ ����
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.State.Dead"));
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);

	//��ŸƮ ����Ʈ ����
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);
		for (auto Effect : StartEffects)
		{
			FGameplayEffectSpecHandle Handle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, Context);
			FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
		}
	}
	SetSpeed(AttributeSet->GetSpeed());


	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || PS->bIsAbilityGiven)
	{
		return;
	}
	PS->bIsAbilityGiven = true;
	//�����Ƽ �ο�
	for (auto ability : PlayerAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(ability, 1, static_cast<int32>(Cast<UGrizzlyAbility>(ability->GetDefaultObject())->InputID), this));
	}
	Grizzly_LOG(  TEXT("End"));
}

void ACPP_Player::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("Confirm"),
			FString("Cancel"), FString("EPGAbilityInputID"), static_cast<int32>(EPGAbilityInputID::Confirm), static_cast<int32>(EPGAbilityInputID::Cancel)));

		ASCInputBound = true;
	}
}

void ACPP_Player::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Leaning"),this, &ACPP_Player::SetRawLeaningAxis);
	BindASCInput();
}

void ACPP_Player::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		//1��Ī ���� ����
		GetMesh()->HideBoneByName(TEXT("bip01_spine"), EPhysBodyOp::PBO_None);
		GetTPWeaponComponent()->SetVisibility(false);
		ShadowModelComponent->SetCastShadow(true);
		ShadowWeaponComponent->SetCastShadow(true);
	}
	else
	{
		GetHandsMeshComponent()->SetVisibility(false, true);

	}

	//�ɱ� ī�޶� �ִϸ��̼��� ���� ���� �ʱ�ȭ
	//���� ī�޶� ����ϴ� �ڵ� > ������ �̻��
	{
		StandingCameraLocation = GetCamera()->GetRelativeLocation();
		CrouchingCameraLocation = StandingCameraLocation;
		CrouchingCameraLocation.Z -= 20.f;
	}

	//ĳ���� �� ����
	if (HasAuthority())
	{
		SetCharacterModel(ECharacterModel::Merc_2a);
	}
}

void ACPP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//���� �����ϰ� �ִ� ���� �ƴ϶�� �Ʒ� ������ �������� �ʴ´�.
	if (!IsLocallyControlled())
	{
		return;
	}
	LeaningCamera(DeltaTime);
	//HandSway(DeltaTime);
	//���� ī�޶� ����� �� ����ϴ� �ڵ�
	//UpdateCameraCrouch(DeltaTime);

}

void ACPP_Player::UpdateCameraCrouch(float _DeltaTime)
{
	FVector CurrentCameraLocation = GetCamera()->GetRelativeLocation();
	FVector TargetCameraLocation;
	float InterpSpeed = 10.f;

	bIsCrouched ? TargetCameraLocation = CrouchingCameraLocation : TargetCameraLocation = StandingCameraLocation;

	CurrentCameraLocation = FMath::VInterpTo(CurrentCameraLocation, TargetCameraLocation, _DeltaTime, InterpSpeed);
	GetCamera()->SetRelativeLocation(CurrentCameraLocation);
}

void ACPP_Player::HandSway(float _DeltaTime)
{
	

	//������ ������ �ִ� �Ķ���ʹ� MouseDelta, HandSwayDelta
	// HandSwayDelta�� ���� ���Ƿ� HandSway�� �ְ� ���� �� ����ϴ� ��
	FRotator StartRotation = FRotator(0.f, -90.f,0.f);
	FRotator TargetRotation = FRotator();
	FRotator CurrentRotation = GetHandsMeshComponent()->GetRelativeRotation();

	float YDelta = FMath::Clamp(MouseYDelta + HandSwayYDelta, -1.f, 1.f);
	float XDelta = FMath::Clamp(MouseXDelta + HandSwayXDelta,-1.f,1.f);
	TargetRotation.Roll = StartRotation.Roll + YDelta * MaxHandSwayDegree;
	TargetRotation.Pitch = StartRotation.Pitch - XDelta * MaxHandSwayDegree;
	TargetRotation.Yaw = StartRotation.Yaw + XDelta * MaxHandSwayDegree;

	//���� �����̼ǿ��� Ÿ�� �����̼Ǳ��� �����Ѵ�
	TargetRotation = FQuat::Slerp(CurrentRotation.Quaternion(), TargetRotation.Quaternion(), _DeltaTime * 4.f).Rotator();

	//������ ȸ���� ���� �� �ִ� ȸ�� ���� �����ϴ� �ڵ�
	float AbsMaxDegree = FMath::Abs(MaxHandSwayDegree);
	TargetRotation.Roll = FMath::Clamp(TargetRotation.Roll, AbsMaxDegree * -1.f, AbsMaxDegree);
	TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, AbsMaxDegree * -1.f, AbsMaxDegree);
	TargetRotation.Yaw = FMath::Clamp(TargetRotation.Yaw, AbsMaxDegree * -1.f + StartRotation.Yaw, AbsMaxDegree + StartRotation.Yaw);

	//HandSwayDelta ���� ���� �پ���
	HandSwayXDelta = FMath::Lerp(HandSwayXDelta, 0.f, _DeltaTime * 15.f);
	HandSwayYDelta = FMath::Lerp(HandSwayYDelta,0.f, _DeltaTime * 15.f);

	GetHandsMeshComponent()->SetRelativeRotation(TargetRotation.Quaternion());
}

void ACPP_Player::UpdateHighReady(float _DeltaTime)
{
	//�Ÿ��� ���� ���̷��� float ����

	//���濡 ���� ����
	// (�������� �Ÿ� / ���̷��� �ߵ� �Ÿ�) < 1.f �� ���̷��� ���·� ����
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	FTransform CameraTransform = GetCamera()->GetComponentTransform();
	FVector ForwardVector = CameraTransform.GetRotation().Vector();
	FVector CameraLocation = CameraTransform.GetLocation();

	//���� ������ �������� ���̷��� �����ϱ����� Start��ġ�� ��¦ ������ �Ѵ�
	FVector StartLocation = CameraLocation + ForwardVector * 30.f;
	FVector EndLocation = CameraLocation + ForwardVector * GetHighReadyDistance();

	FHitResult HitResult;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),StartLocation,EndLocation,TraceType,false,{},EDrawDebugTrace::None,
		HitResult,true);
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.HighReady")));
	if (!bHit)
	{
		HighReady = 1.f;
		bHighReady = false;
		HighReadyInterpo = 1.f;
		return;
	}

	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);
	float Distance = FVector::Distance(HitResult.ImpactPoint, CameraLocation);
	HighReady = Distance / GetHighReadyDistance();
	bHighReady = true;


}

void ACPP_Player::UpdateHighReadyInterpo(float _DeltaTime)
{
	//���� ���� ���̷�����¿��� ������ �ϸ� ���� �� �ڷ���Ʈ��. 
	// �װ��� �����ϱ� ���� GetCurrentHandsLocation �� ��ȭ�ϸ� �� ���� ���� �̿��Ѵ�
	CurrentHandsLocationInterpo = FMath::VInterpTo(CurrentHandsLocationInterpo,GetCurrentHandsLocation(),_DeltaTime,5.f);
	if(!bHighReady)
		return;

	// ���� ���̷��� ��������
	HighReadyInterpo = FMath::FInterpTo(HighReadyInterpo,HighReady,_DeltaTime,10.f);
	FVector TargetLocation = FMath::Lerp(GetHighReadyLocation(), CurrentHandsLocationInterpo,HighReadyInterpo);
	FRotator TargetRotation =FQuat::Slerp(GetHighReadyRotation().Quaternion(), FRotator(0, -90.f, 0).Quaternion(), HighReadyInterpo).Rotator();
	GetHandsMeshComponent()->SetRelativeLocationAndRotation(TargetLocation, TargetRotation);
}

void ACPP_Player::LeaningCamera(float _DeltaTime)
{
	FRotator SourceRotation = GetControlRotation();
	FRotator TargetRotation = FRotator(SourceRotation.Pitch,SourceRotation.Yaw, GetLeaningAxis() * 10.f);
	TargetRotation = FQuat::Slerp(SourceRotation.Quaternion(), TargetRotation.Quaternion(), _DeltaTime * 6.f).Rotator();
	GetController()->SetControlRotation(TargetRotation);
	
}

void ACPP_Player::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	//���� ī�޶� ����϶� ����ϴ� �ڵ�
	//FVector CurrentCameraLocation = StandingCameraLocation;
	//float Padding = 10.f;
	//CurrentCameraLocation.Z += ScaledHalfHeightAdjust + Padding;
	//GetCamera()->SetRelativeLocation(CurrentCameraLocation);
}

void ACPP_Player::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	//���� ī�޶� ����϶� ����ϴ� �ڵ�
	//FVector CurrentCameraLocation = StandingCameraLocation;
	//float Padding = 10.f;
	//CurrentCameraLocation.Z -= ScaledHalfHeightAdjust + Padding;
	//GetCamera()->SetRelativeLocation(CurrentCameraLocation);
}

EFaction ACPP_Player::GetFaction() const
{
	//��ü�� �Ǹ� PS ĳ�����ϸ� nullptr ����. �̰��� ����ó���� �ؾ� ��
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	if(PS == nullptr)
	{
		return EFaction::NonSet;
	}
	return PS->GetFaction();
}

UFactionComponent* ACPP_Player::GetFactionComponent() const
{
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	return PS->GetFactionComponent();
}
