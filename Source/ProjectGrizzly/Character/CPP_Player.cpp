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
#include "AbilitySystemBlueprintLibrary.h"

#include "Ability/CPP_GA_SwapWeapon.h"
#include "GameFramework/InputSettings.h"
#include "Net\UnrealNetwork.h"

#include "ProjectGrizzly/ProjectGrizzly.h"
#include "ProjectGrizzly/Item/CPP_Crate.h"
#include "ProjectGrizzly/Item/Interactable.h"

ACPP_Player::ACPP_Player()
{
	UsableItemMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UsableItemMeshComponent"));
	UsableItemMeshComponent->SetupAttachment(GetHandsMeshComponent());
	UsableItemMeshComponent->SetIsReplicated(true);
	UsableItemMeshComponent->SetVisibility(false);
}

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

	//���� �ʱ�ȭ
	GetInventory()->InitWeaponInstanceToUnarmedInstance();

	//�κ��丮 �ʱ�ȭ
	if(AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetController()); PC)
	{
		PC->LoadPlayerInventoryFromInstance();
	}
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
	for (auto Effect : StartEffects)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(Effect,GetAbilitySystemComponent());
	}
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
	
	//�̱۸��
	if(IsLocallyControlled())
	{
		//���� �ʱ�ȭ
		GetInventory()->InitWeaponInstanceToUnarmedInstance();
	}
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
	PlayerInputComponent->BindAction(TEXT("SwapWeapon"),IE_Pressed,this,&ACPP_Player::OnCurrentWeaponSlot);
	PlayerInputComponent->BindAction(TEXT("Interaction"),IE_Pressed,this,&ACPP_Player::TryInteract);
	BindASCInput();
}

void ACPP_Player::BeginPlay()
{
	Super::BeginPlay();


	
	if (IsLocallyControlled())
	{
		//1��Ī ���� ����
		GetMesh()->HideBoneByName(TEXT("bip01"), EPhysBodyOp::PBO_None);
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


	if (HasAuthority())
	{
		// ĳ���� �� �ʱ�ȭ
		SetCharacterModel(CharacterModel);
	}
}

void ACPP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//���� �����ϰ� �ִ� ���� �ƴ϶�� �Ʒ� ������ �������� �ʴ´�.
	if (!IsLocallyControlled() || !GetAbilitySystemComponent())
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

void ACPP_Player::UpdateHighReady(TPair<bool,FHitResult>& _HitPair)
{
	bool bHit = _HitPair.Key;
	FHitResult HitResult = _HitPair.Value;
	
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.HighReady")));
	if (!bHit)
	{
		HighReady = 1.f;
		bHighReady = false;
		HighReadyInterpo = 1.f;
		return;
	}
	FTransform CameraTransform = GetCamera()->GetComponentTransform();
	FVector ForwardVector = CameraTransform.GetRotation().Vector();
	FVector CameraLocation = CameraTransform.GetLocation();
	
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);
	float Distance = FVector::Distance(HitResult.ImpactPoint, CameraLocation);
	HighReady = Distance / GetInteractionDistance();
	bHighReady = true;
}

bool ACPP_Player::CanInteractWithTarget(TPair<bool, FHitResult>& HitPair) const
{
	const FHitResult HitResult = HitPair.Value;
	if(!HitPair.Key)
		return false;
	AActor* HitActor = HitResult.GetActor();
	if(HitActor == nullptr)
		return false;
	if(!HitActor->ActorHasTag(TEXT("Interactable")))
	{
		return false;
	}
	const AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetController());
	if(!PC)
	{
		return false;
	}

	//�κ��丮 UI�� �����ִ� ���¿��� ��ȣ�ۿ��� �Ұ���
	const UUserWidget* InventoryUI = PC->GetInventoryUI();
	if(!InventoryUI)
	{
		return false;
	}
	if(InventoryUI->IsVisible())
		return false;


	const IInteractable* Interaction = Cast<IInteractable>(HitActor);
	if(!Interaction)
	{
		return false;
	}
	if(!Interaction->Execute_CanInteract(HitActor))
	{
		return false;
	}

	
	return true;
}

TPair<bool,FHitResult> ACPP_Player::GetFrontHitResult() const
{
	//���濡 ���� ����
	// (�������� �Ÿ� / ��ȣ�ۿ� �ߵ� �Ÿ�) < 1.f �� ��ȣ�ۿ� ���·� ����
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	FTransform CameraTransform = GetCamera()->GetComponentTransform();
	FVector ForwardVector = CameraTransform.GetRotation().Vector();
	FVector CameraLocation = CameraTransform.GetLocation();

	//���� ������ �������� �ڿ� ��ü�� Ȯ���ϴ°��� ����
	FVector StartLocation = CameraLocation + ForwardVector * 30.f;
	FVector EndLocation = CameraLocation + ForwardVector * GetInteractionDistance();

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation,ECC_Visibility,QueryParams);
	// bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),StartLocation,EndLocation,TraceType,false,{},EDrawDebugTrace::None,
	//                                                   HitResult,true);
	// if(bHit)
	return TPair<bool,FHitResult>{bHit,HitResult};
}

void ACPP_Player::UpdateFrontInteraction(float _DeltaTime)
{
	TPair<bool,FHitResult> HitPair = GetFrontHitResult();
	UpdateHighReady(HitPair);

	//���� �����ӿ� ���̿� ��Ʈ�� ������Ʈ�� ����
	if( PrevInteractableObject != nullptr)
	{
		//��Ʈ�� ������Ʈ�� ���� ������ ������Ʈ�� �ٸ��� ���� ������Ʈ�� UI ��Ȱ��ȭ
		if(Cast<IInteractable>(HitPair.Value.GetActor()) != PrevInteractableObject)
		{
			if(AActor* InteractableObject = Cast<AActor>(PrevInteractableObject);InteractableObject)
			{
				PrevInteractableObject->Execute_CloseWidget(InteractableObject);
				PrevInteractableObject = nullptr;
			}
		}
	}
	//��ȣ�ۿ� �Ұ��� ���¸� UI�� ����
	if(!CanInteractWithTarget(HitPair))
	{
		if(AActor* InteractableObject = Cast<AActor>(PrevInteractableObject);InteractableObject)
		{
			PrevInteractableObject->Execute_CloseWidget(InteractableObject);
		}
		return;
	}

	//��Ʈ�� ������Ʈ�� ��ȣ�ۿ� ����

	//��ȣ�ۿ� ������ ������Ʈ�� �ִٰ� UI�� ����
	IInteractable* InteractableObject =Cast<IInteractable>(HitPair.Value.GetActor());
	InteractableObject->Execute_ShowWidget(HitPair.Value.GetActor());
	PrevInteractableObject = InteractableObject;
}

void ACPP_Player::TryInteract()
{
	if(PrevInteractableObject == nullptr)
		return;
	AActor* InteractableObject = Cast<AActor>(PrevInteractableObject);
	
	if(!InteractableObject)
		return;
	if(!PrevInteractableObject->Execute_CanInteract(InteractableObject))
		return;
	
	PrevInteractableObject->Execute_Interact(InteractableObject,this);
}

void ACPP_Player::TryUseItem(ACPP_Item* _Item)
{
	FGameplayEventData EventData;
	EventData.Instigator = this;
	EventData.Target = this;
	EventData.OptionalObject = _Item;
	GetAbilitySystemComponent()->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(TEXT("Ability.State.UseItem")),&EventData);
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
	if(PS == nullptr)
	{
		return nullptr;
	}
	return PS->GetFactionComponent();
}


void ACPP_Player::OnCurrentWeaponSlot()
{
	//Ŭ���̾�Ʈ
	UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	if(!InputSettings)
		return;

	//������Ʈ �Է��� ��ǲ �׼� �迭�� ������
	TArray<FInputActionKeyMapping> Mappings;
	InputSettings->GetActionMappingByName(TEXT("SwapWeapon"),Mappings);
	Algo::Reverse(Mappings);

	//�迭�� Ű���� ��� ���� Ű�� ��
	for(int i = 0; i < Mappings.Num() ; i++)
	{
		if(GetWorld()->GetFirstPlayerController()->PlayerInput->IsPressed(Mappings[i].Key))
		{
			EWeaponSlot Slot = static_cast<EWeaponSlot>(i+1);
			SetCurrentWeaponSlot(Slot);
		}
	}
}

ACPP_WeaponInstance* ACPP_Player::GetCurrentWeaponInstance()
{
	return 	GetWeaponComponent()->GetCurrentWeaponInstance();
}

ACPP_WeaponInstance* ACPP_Player::GetWeaponInstanceFromSlot(EWeaponSlot _Slot)
{
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetController());
	checkf(PC,TEXT("PC is invalid"));
	
	UPlayerInventoryComponent* Inventory = PC->GetInventoryComponent();
	ACPP_WeaponInstance* WeaponInstance = Inventory->GetWeaponInstanceFromSlot(_Slot);
	check(WeaponInstance);
	return WeaponInstance;
}

void ACPP_Player::TryActivateSwapWeapon(EWeaponSlot _Slot)
{
	Super::TryActivateSwapWeapon(_Slot);
	SetCurrentWeaponSlot(_Slot);
	GetAbilitySystemComponent()->TryActivateAbilityByClass(UCPP_GA_SwapWeapon::StaticClass());
}

void ACPP_Player::SwapWeapon(EWeaponSlot _Slot)
{
	Super::SwapWeapon(_Slot);
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(GetController());
	checkf(PC,TEXT("PC is invalid"));
	UPlayerInventoryComponent* Inventory = PC->GetInventoryComponent();
	Inventory->SwapWeaponBySlot(_Slot);
}

