#include "CPP_PlayableCharacter.h"
#include <GameFramework\SpringArmComponent.h>
#include <Camera\CameraComponent.h>
#include "CPP_GrizzlyPS.h"
#include "Ability\GrizzlyAbility.h"
#include "AbilitySystemComponent.h"
#include "GrizzlyPC.h"
#include <Kismet/KismetMathLibrary.h>
#include "PlayerInventoryComponent.h"
#include <Components/CapsuleComponent.h>
#include "..\Weapon\WeaponComponent.h"
#include "Net\UnrealNetwork.h"
#include "Ability/CPP_GA_Leaning.h"
#include "ProjectGrizzly/ProjectGrizzly.h"
//ToDo : AI������ ������ ���
#include "AIModule\Classes\AIController.h"

ACPP_PlayableCharacter::ACPP_PlayableCharacter()
{
	GetCapsuleComponent()->SetCapsuleRadius(45.0f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.f);
	GetMesh()->SetRelativeLocation({ -16.f,0,-90.f });
	GetMesh()->SetRelativeRotation(FRotator(0, 270.f, 0).Quaternion());
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetIsReplicated(true);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetRelativeLocation({ 0,0,0.6f });
	Camera->SetFieldOfView(70.f);
	Camera->bUsePawnControlRotation = true;
	//Camera->SetupAttachment(RootComponent);
	Camera->SetupAttachment(GetMesh(), TEXT("bip01_head"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 0.f;
	SpringArm->SetupAttachment(Camera);

	HandsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMeshComponent"));
	HandsMeshComponent->SetRelativeLocation( GetIdleHandsLocation());
	HandsMeshComponent->SetRelativeRotation(GetHandsRotation().Quaternion());
	//static auto HandsMesh = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Game/ProjectGrizzly/Character/FirstPersonHand/HandMesh/Merc_Metro/hand_merc_metro.hand_merc_metro"));
	//HandsMeshComponent->SetSkeletalMesh(HandsMesh.Object);
	//static auto HandsMeshClass = ConstructorHelpers::FClassFinder<UAnimInstance>(TEXT("/Game/ProjectGrizzly/Character/FirstPersonHand/ABP_Hand.ABP_Hand_C"));
	HandsMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//HandsMeshComponent->SetAnimInstanceClass(HandsMeshClass.Class);
	HandsMeshComponent->SetBoundsScale(1.5f);
	HandsMeshComponent->SetupAttachment(SpringArm);


	FPWeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPWeaponMeshComponent"));
	FPWeaponMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	FPWeaponMeshComponent->SetBoundsScale(3.f); //���� ī�޶� �޶� �پ��ֱ� ������ �ٿ�� �������� Ű���� ������ Ŭ���� �Ǵ� ���װ� ����
	FPWeaponMeshComponent->SetupAttachment(HandsMeshComponent);

	//�׸���
	ShadowModelComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowModelComponent"));
	ShadowModelComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//ShadowModelComponent->SetAnimInstanceClass(CharacterMeshClass.Class);
	ShadowModelComponent->SetRelativeLocation({ -16.f,0,-90.f });
	ShadowModelComponent->SetRelativeRotation(FRotator(0, 270.f, 0).Quaternion());
	ShadowModelComponent->bRenderInMainPass = false;
	ShadowModelComponent->SetCastShadow(false);
	ShadowModelComponent->SetIsReplicated(false);
	ShadowModelComponent->SetupAttachment(RootComponent);


	ShadowWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowWeaponComponent"));
	ShadowWeaponComponent->bRenderInMainPass = false;
	ShadowWeaponComponent->SetCastShadow(false);
	ShadowWeaponComponent->SetIsReplicated(false);
	ShadowWeaponComponent->SetupAttachment(ShadowModelComponent, TEXT("WeaponRight"));



	TPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArm"));
	TPSpringArm->SetRelativeLocation({ -16.f,0,0 });
	TPSpringArm->SetupAttachment(RootComponent);
	TPSpringArm->TargetArmLength = 100.f;

	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	TPCamera->SetupAttachment(TPSpringArm);

	TPWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TPWeaponComponent"));
	TPWeaponComponent->SetupAttachment(GetMesh(), TEXT("WeaponRight"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetIsReplicated(true);
	RemoteViewPitch = true;


	//�κ��丮
	InventoryComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
	Tags.Add(TEXT("Interactable"));

	//�ʵ� ���� UI
	FieldLootUIWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FieldLootUI"));
	FieldLootUIWidgetComponent->SetupAttachment(GetMesh());
	FieldLootUIWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	FieldLootUIWidgetComponent->SetRelativeLocation({0,0,100});

	static auto FieldLootUI = ConstructorHelpers::FClassFinder<UUserWidget>(TEXT("/Game/ProjectGrizzly/Item/BP_FieldLootUI.BP_FieldLootUI_C"));
	if(FieldLootUI.Succeeded())
	{
		FieldLootUIWidgetComponent->SetWidgetClass(FieldLootUI.Class);
		FieldLootUIWidgetComponent->SetVisibility(false);
	}

}
float ACPP_PlayableCharacter::GetLeaningAxis() const
{
	return LeaningAxis;
	//if(!ASCInputBound)
	//	return 0.f;
	////Leaning �����Ƽ �ν��Ͻ��� LeaningAxis ����
	//FGameplayAbilitySpec* LeaningAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromInputID(static_cast<int32>(EPGAbilityInputID::Leaning));
	//if (!LeaningAbilitySpec)
	//	return 0.f;
	//UCPP_GA_Leaning* LeaningAbility = Cast<UCPP_GA_Leaning>(LeaningAbilitySpec->GetAbilityInstances()[0]);
	//if (!LeaningAbility)
	//	return 0.f;
	//return LeaningAbility->GetLeaningAxis();
}
void ACPP_PlayableCharacter::SetLeaningAxis(float _LeaningAxis)
{
	LeaningAxis = _LeaningAxis;
}
void ACPP_PlayableCharacter::SetRawLeaningAxis_Server_Implementation(float _LeaningAxis)
{
	RawLeaningAxis = _LeaningAxis;
}
void ACPP_PlayableCharacter::SetRawLeaningAxis(float _LeaningAxis)
{
	//_LeaningAxis�� ��ȭ�� �������� ������ �����͸� �����Ѵ�.
	if (_LeaningAxis != RawLeaningAxis)
	{
		SetRawLeaningAxis_Server(_LeaningAxis);
	}
	RawLeaningAxis = _LeaningAxis;
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
	DOREPLIFETIME(ACPP_PlayableCharacter, CharacterModel);
	DOREPLIFETIME(ACPP_PlayableCharacter, LeaningAxis);
	DOREPLIFETIME(ACPP_PlayableCharacter, CurrentWeaponSlot);
	

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



bool ACPP_PlayableCharacter::IsUnarmed(ACPP_WeaponInstance* _WeaponInstance) const
{
	return GetWeaponComponent()->GetUnarmedInstance() == _WeaponInstance;
}

bool ACPP_PlayableCharacter::CanInteract_Implementation() const
{
	return IsDead();
}

void ACPP_PlayableCharacter::Interact_Implementation(class AActor* _Instigator)
{
	IInteractable::Interact_Implementation(_Instigator);
	AGrizzlyPC* PC = Cast<AGrizzlyPC>(_Instigator->GetOwner());
	if(!PC)
		return;
	
	FInventoryUI_Parameter UI_Parameter;
	UI_Parameter.Target = GetInventory();
	UI_Parameter.InventoryCategory = EInventoryCategory::ItemContainer;
	PC->ShowInventory(UI_Parameter);
}

void ACPP_PlayableCharacter::ShowWidget_Implementation()
{
	IInteractable::ShowWidget_Implementation();
	FieldLootUIWidgetComponent->SetVisibility(true);
}

void ACPP_PlayableCharacter::CloseWidget_Implementation()
{
	IInteractable::CloseWidget_Implementation();
	FieldLootUIWidgetComponent->SetVisibility(false);
}

void ACPP_PlayableCharacter::SetCurrentWeaponSlot_Implementation(EWeaponSlot _CurrentWeaponSlot)
{
	CurrentWeaponSlot = _CurrentWeaponSlot;
	//FieldLootUIWidgetComponent->SetVisibility(false);
}


void ACPP_PlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateBendDownDegree();
	UpdateADS(DeltaTime);
	if (IsMyComputer())
	{

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
	FVector InterpoValue =GetADSHandsLocation();
	float OriginDistance = FVector::Distance(GetADSHandsLocation(),  GetIdleHandsLocation());

	if (bADS)
	{
		InterpoValue = FMath::VInterpTo(CurrentLocation, GetADSHandsLocation(), DeltaTime, GetADSSpeed());
		FRotator InterpoRotator = FMath::RInterpTo(Hands->GetRelativeRotation(), GetHandsRotation(), DeltaTime, GetADSSpeed());
		float Distance = FVector::Distance(InterpoValue, GetIdleHandsLocation());
		ADSFactor = Distance / OriginDistance;
		Hands->SetRelativeLocation(InterpoValue);

		Hands->SetRelativeRotation(InterpoRotator);
	}
	else
	{
		InterpoValue = FMath::VInterpTo(CurrentLocation,  GetIdleHandsLocation(), DeltaTime, GetADSSpeed());
		float Distance = FVector::Distance(InterpoValue,  GetIdleHandsLocation());
		ADSFactor = Distance / OriginDistance;
		// Except : ������ �� ������ ĵ���� �� ��� ������ �̵�

		//���� ���� �� Ǯ�� �����Ƽ ��Ȱ��ȭ
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
	//float Distance = FVector::Distance(InterpoValue, IdleHandLocation);
	//ADSFactor = Distance / OriginDistance;
	//Hands->SetRelativeLocation(InterpoValue);
}



class UCameraComponent* ACPP_PlayableCharacter::GetCamera() const
{
	return Camera;
}

void ACPP_PlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	TPCamera->SetActive(false);
	
	//���� ���۽� ����Ʈ�� Unarmed
	GetWeaponComponent()->SetUnarmed();
	
	if (!IsLocallyControlled())
	{
		ShadowModelComponent->SetActive(false);
		ShadowWeaponComponent->SetActive(false);
	}
}

void ACPP_PlayableCharacter::OnRep_SetCharacterModel()
{
	const UEnum* CharacterModelEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECharacterModel"), true);
	ensure(CharacterModelEnum);
	FName EnumToName(CharacterModelEnum->GetDisplayNameTextByValue((int64)CharacterModel).ToString());

	static const UDataTable* DT_CHARACTERMODEL = LoadObject<UDataTable>(NULL, TEXT("/Game/ProjectGrizzly/Character/DT_CharacterModel.DT_CharacterModel"));

	FCharacterModel* Model = DT_CHARACTERMODEL->FindRow<FCharacterModel>(EnumToName, FString(""));
	ensure(Model); //_Model�� NonSet�Դϴ�

	if (Model->Character.IsPending()) //������ �ε������� ���� ��� ���� �ε�
	{
		Model->Character.LoadSynchronous();
		Model->Character.Get();
	}

	//ĳ���� �𵨷� ����
	GetMesh()->SetSkeletalMesh(Model->Character.Get());
	ShadowModelComponent->SetSkeletalMesh(Model->Character.Get());
}


void ACPP_PlayableCharacter::SetCharacterModel(ECharacterModel _Model)
{
	//���� & �̱� ����ϰ�� ������Ƽ�� �����ϰ� OnRep�Լ� ȣ��
	if (HasAuthority())
	{
		CharacterModel = _Model;
		OnRep_SetCharacterModel();
		return;
	}

	//Ŭ���̾�Ʈ�ϰ�� ������ ������Ƽ ���� ��û
	SetCharacterModel_Server(_Model);
}

void ACPP_PlayableCharacter::SetCharacterModel_Server_Implementation(ECharacterModel _Model)
{
	CharacterModel = _Model;
	OnRep_SetCharacterModel();
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

void ACPP_PlayableCharacter::ReturnToHands(float _DeltaTime)
{
	// ������ ���¸� ���� �Լ����� �����ؿ� ���� Ʈ�������� ������
	// ���̷��� ������
	if (bADS || bHighReady)
	{
		return;
	}

	FTransform CurrentTransform = GetHandsMeshComponent()->GetRelativeTransform();
	//��ġ ����
	float InterpoSpeed = 10.f;
	FVector InterpoLocation = FMath::VInterpTo(CurrentTransform.GetLocation(),GetCurrentHandsLocation(),_DeltaTime,InterpoSpeed);
	GetHandsMeshComponent()->SetRelativeLocation(InterpoLocation);
	// ȸ�� ����
	FRotator IdleRotator = GetHandsRotation();
	FRotator InterpoRotator = FMath::RInterpTo(CurrentTransform.Rotator(), IdleRotator,_DeltaTime,InterpoSpeed);
	GetHandsMeshComponent()->SetRelativeRotation(InterpoRotator);
}


float ACPP_PlayableCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
	return ActualDamage;
}

void ACPP_PlayableCharacter::Die()
{
	Super::Die();
	//Die ����Ʈ ����
	FGameplayEffectContextHandle Context = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DeadGameplayEffect, 1, Context);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetSimulatePhysics(true);

	//�κ��丮
	GetInventory()->UnEquipWeapon(GetInventory()->GetWeaponInstanceFromSlot(EWeaponSlot::Secondary));
	GetInventory()->UnEquipWeapon(GetInventory()->GetWeaponInstanceFromSlot(EWeaponSlot::Primary));
	GetTPWeaponComponent()->SetVisibility(false);
	Die_Multicast();
}

void ACPP_PlayableCharacter::Die_Multicast_Implementation()
{
	//�������� �ӽ��� ���
	if (IsLocallyControlled() && !bIsAI)
	{
		//����� 3��Ī ĳ���� ����� ���̰� & 1��Ī �޽� ����
		DeadLocation = GetMesh()->GetSocketLocation(TEXT("Pelvis"));
		HandsMeshComponent->SetVisibility(false);
		FPWeaponMeshComponent->SetVisibility(false);
		GetTPWeaponComponent()->SetVisibility(true);
		GetMesh()->UnHideBoneByName(TEXT("bip01_spine"));
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		

		//ī�޶� ����
		Camera->SetActive(false);
		TPCamera->SetActive(true);
		float xPos = FMath::RandRange(-100.f, 100.f);
		float yPos = FMath::RandRange(-100.f, 100.f);
		float zPos = FMath::RandRange(-20.f, 100.f);
		TPSpringArm->SetRelativeLocation({ xPos,yPos,zPos }, true);
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TPSpringArm->GetComponentLocation(), DeadLocation);
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
		return true;
	}
	return !PS->IsAlive();
}

void ACPP_PlayableCharacter::UpdateDeathCam(float _DeltaTime)
{
	DeathCamSpeed = FMath::FInterpTo(DeathCamSpeed, 0, _DeltaTime, 5.0);
	FVector LookAtDir = TPSpringArm->GetComponentLocation() - DeadLocation;
	LookAtDir.Normalize();
	TPSpringArm->AddWorldOffset(LookAtDir * DeathCamSpeed, true);
}

class USkeletalMeshComponent* ACPP_PlayableCharacter::GetFPWeaponMeshComponent()
{
	return FPWeaponMeshComponent;
}


