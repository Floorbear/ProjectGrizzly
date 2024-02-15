
#include "CPP_PlayableCharacter.h"
#include <GameFramework\SpringArmComponent.h>
#include <Camera\CameraComponent.h>
#include "CPP_GrizzlyPS.h"
#include "Ability\GrizzlyAbility.h"
#include "AbilitySystemComponent.h"

#include "ProjectGrizzly/ProjectGrizzly.h"

ACPP_PlayableCharacter::ACPP_PlayableCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(),TEXT("bip01_head"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	HandsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMeshComponent"));
	HandsMeshComponent->SetupAttachment(Camera);

	FPWeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPWeaponMeshComponent"));
	FPWeaponMeshComponent->SetupAttachment(HandsMeshComponent);
}

void ACPP_PlayableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ACPP_PlayableCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	BindASCInput();
}

void ACPP_PlayableCharacter::BindASCInput()
{
	/*if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EGDAbilityInputID"), static_cast<int32>(EGDAbilityInputID::Confirm), static_cast<int32>(EGDAbilityInputID::Cancel)));

		ASCInputBound = true;
	}*/
	if (!ASCInputBound && IsValid(AbilitySystemComponent) && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("Confirm"),
			FString("Cancel"), FString("EPGAbilityInputID"), static_cast<int32>(EPGAbilityInputID::Confirm), static_cast<int32>(EPGAbilityInputID::Cancel)));

		ASCInputBound = true;
	}
}

void ACPP_PlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsMyComputer())
	{
		GetMesh()->HideBoneByName(TEXT("bip01_l_clavicle"),EPhysBodyOp::PBO_None);
		GetMesh()->HideBoneByName(TEXT("bip01_r_clavicle"), EPhysBodyOp::PBO_None);
		/*FTimerHandle NewTimer;
		GetWorld()->GetTimerManager().SetTimer(NewTimer, FTimerDelegate::CreateLambda([&]() 
			{
				
			}
		), 0.015, true);*/
	}
	else
	{
		GetHandsMeshComponent()->SetVisibility(false,true);
	}
}


// On Client
void ACPP_PlayableCharacter::OnRep_PlayerState()
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
}

void ACPP_PlayableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ACPP_GrizzlyPS* PS = Cast<ACPP_GrizzlyPS>(GetPlayerState());
	if (!PS)
	{
		return;
	}

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();
	//InitializeAttributes();
		// Grant abilities, but only on the server	

	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent))
	{
		return;
	}

	//AbilitySystemComponent->GiveAbility(
	//	FGameplayAbilitySpec(UCPP_GA_Reload::StaticClass(),1, static_cast<int32>(EPGAbilityInputID::Reload), this));
	for (auto ability : PlayerAbilies)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(ability, 1, static_cast<int32>(Cast<UGrizzlyAbility>(ability->GetDefaultObject())->InputID), this));
	}

}

class USkeletalMeshComponent* ACPP_PlayableCharacter::GetHandsMeshComponent()
{
	return HandsMeshComponent;
}

class USkeletalMeshComponent* ACPP_PlayableCharacter::GetFPWeaponMeshComponent()
{
	return FPWeaponMeshComponent;
}

