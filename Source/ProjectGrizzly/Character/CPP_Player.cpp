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

//Client만 호출
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

// 서버의 Owner 값이 변경되서 클라이언트에 복제될 때 호출되는 이벤트함수
// 서버에서만 호출된다
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


//Only 서버 호출
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

	//(리스폰 할 경우) 이전 이펙트 삭제
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.State.Dead"));
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);

	//스타트 이펙트 적용
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
	//어빌리티 부여
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
		//1인칭 셋팅 진행
		GetMesh()->HideBoneByName(TEXT("bip01_spine"), EPhysBodyOp::PBO_None);
		GetTPWeaponComponent()->SetVisibility(false);
		ShadowModelComponent->SetCastShadow(true);
		ShadowWeaponComponent->SetCastShadow(true);
	}
	else
	{
		GetHandsMeshComponent()->SetVisibility(false, true);

	}

	//앉기 카메라 애니메이션을 위한 변수 초기화
	//고정 카메라때 사용하던 코드 > 지금은 미사용
	{
		StandingCameraLocation = GetCamera()->GetRelativeLocation();
		CrouchingCameraLocation = StandingCameraLocation;
		CrouchingCameraLocation.Z -= 20.f;
	}

	//캐릭터 모델 설정
	if (HasAuthority())
	{
		SetCharacterModel(ECharacterModel::Merc_2a);
	}
}

void ACPP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//내가 조작하고 있는 폰이 아니라면 아래 로직을 실행하지 않는다.
	if (!IsLocallyControlled())
	{
		return;
	}
	LeaningCamera(DeltaTime);
	//HandSway(DeltaTime);
	//고정 카메라 모드일 때 사용하던 코드
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
	

	//보간에 영햐을 주는 파라미터는 MouseDelta, HandSwayDelta
	// HandSwayDelta는 내가 임의로 HandSway를 주고 싶을 때 사용하는 값
	FRotator StartRotation = FRotator(0.f, -90.f,0.f);
	FRotator TargetRotation = FRotator();
	FRotator CurrentRotation = GetHandsMeshComponent()->GetRelativeRotation();

	float YDelta = FMath::Clamp(MouseYDelta + HandSwayYDelta, -1.f, 1.f);
	float XDelta = FMath::Clamp(MouseXDelta + HandSwayXDelta,-1.f,1.f);
	TargetRotation.Roll = StartRotation.Roll + YDelta * MaxHandSwayDegree;
	TargetRotation.Pitch = StartRotation.Pitch - XDelta * MaxHandSwayDegree;
	TargetRotation.Yaw = StartRotation.Yaw + XDelta * MaxHandSwayDegree;

	//현재 로테이션에서 타겟 로테이션까지 보간한다
	TargetRotation = FQuat::Slerp(CurrentRotation.Quaternion(), TargetRotation.Quaternion(), _DeltaTime * 4.f).Rotator();

	//과도한 회전이 들어올 때 최대 회전 각을 제한하는 코드
	float AbsMaxDegree = FMath::Abs(MaxHandSwayDegree);
	TargetRotation.Roll = FMath::Clamp(TargetRotation.Roll, AbsMaxDegree * -1.f, AbsMaxDegree);
	TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, AbsMaxDegree * -1.f, AbsMaxDegree);
	TargetRotation.Yaw = FMath::Clamp(TargetRotation.Yaw, AbsMaxDegree * -1.f + StartRotation.Yaw, AbsMaxDegree + StartRotation.Yaw);

	//HandSwayDelta 값은 점점 줄어든다
	HandSwayXDelta = FMath::Lerp(HandSwayXDelta, 0.f, _DeltaTime * 15.f);
	HandSwayYDelta = FMath::Lerp(HandSwayYDelta,0.f, _DeltaTime * 15.f);

	GetHandsMeshComponent()->SetRelativeRotation(TargetRotation.Quaternion());
}

void ACPP_Player::UpdateHighReady(float _DeltaTime)
{
	//거리에 따른 하이레디 float 설정

	//전방에 빔을 쏴서
	// (벽까지의 거리 / 하이레디 발동 거리) < 1.f 면 하이레디 상태로 진입
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	FTransform CameraTransform = GetCamera()->GetComponentTransform();
	FVector ForwardVector = CameraTransform.GetRotation().Vector();
	FVector CameraLocation = CameraTransform.GetLocation();

	//벽을 등지고 딱붙을때 하이레디를 방지하기위해 Start위치를 살짝 앞으로 한다
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
	//조준 상태 하이레디상태에서 정조준 하면 손이 걍 텔레포트함. 
	// 그것을 방지하기 위해 GetCurrentHandsLocation 가 변화하면 그 보간 값을 이용한다
	CurrentHandsLocationInterpo = FMath::VInterpTo(CurrentHandsLocationInterpo,GetCurrentHandsLocation(),_DeltaTime,5.f);
	if(!bHighReady)
		return;

	// 현재 하이레디 보간정도
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
	//고정 카메라 모드일때 사용하던 코드
	//FVector CurrentCameraLocation = StandingCameraLocation;
	//float Padding = 10.f;
	//CurrentCameraLocation.Z += ScaledHalfHeightAdjust + Padding;
	//GetCamera()->SetRelativeLocation(CurrentCameraLocation);
}

void ACPP_Player::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	//고정 카메라 모드일때 사용하던 코드
	//FVector CurrentCameraLocation = StandingCameraLocation;
	//float Padding = 10.f;
	//CurrentCameraLocation.Z -= ScaledHalfHeightAdjust + Padding;
	//GetCamera()->SetRelativeLocation(CurrentCameraLocation);
}

EFaction ACPP_Player::GetFaction() const
{
	//시체가 되면 PS 캐스팅하면 nullptr 리턴. 이것의 예외처리를 해야 함
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
