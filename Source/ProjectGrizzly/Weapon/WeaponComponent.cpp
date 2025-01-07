// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "..\Character\CPP_PlayableCharacter.h"
#include "..\Character\CPP_PGCharacter.h"
#include "..\Character\CPP_A_Hands.h"
#include "Net\UnrealNetwork.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//ToDo : DT to GameInstance
	static auto DataTable =  ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponDataTable.WeaponDataTable"));
	WeaponDataTable = DataTable.Object;

	static auto AnimDataTable = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/ProjectGrizzly/Gun/WeaponAnimTable.WeaponAnimTable"));
	WeaponAnimTable = AnimDataTable.Object;


	//3��Ī �ִϸ��̼� ToDo : 3��Ī �ִϸ��̼��� ��� ���� �ε�
	static auto TPADSShootData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_ADSShoot.AM_Torso2_ADSShoot"));
	TP_MontageMap.Add(TEXT("ADSShoot"), TPADSShootData.Object);

	static auto TPHipShootData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_HipShoot.AM_Torso2_HipShoot"));
	TP_MontageMap.Add(TEXT("HipShoot"), TPHipShootData.Object);

	static auto ReloadData = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_Reload.AM_Torso2_Reload"));
	TP_MontageMap.Add(TEXT("Reload"), ReloadData.Object);

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_CrouchReload.AM_Torso2_CrouchReload"));
		TP_MontageMap.Add(TEXT("Crouch_Reload"), Data.Object);
	}

	{
		static auto Data = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/ProjectGrizzly/Character/Model/Animations/AM_Torso2_CrouchShoot.AM_Torso2_CrouchShoot"));
		TP_MontageMap.Add(TEXT("Crouch_Shoot"), Data.Object);
	}

	//������Ʈ�� ���ø����̼� ������ ���Ϳ��� �Ѵ�


}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	WeaponData = GetWeaponDataTable()->FindRow<FWeaponData>(WeaponName, FString(""));
	ensure(WeaponData != NULL); // WeaponName �� �ش��ϴ� �����Ͱ� �������� ����

	WeaponAnim = GetWeaponAnimTable()->FindRow<FWeaponAnim>(WeaponName, FString(""));
	ensure(WeaponData != NULL); // WeaponName �� �ش��ϴ� �����Ͱ� �������� ����

	InitWeaponData();

}


void UWeaponComponent::InitWeaponData()
{
	ACPP_PlayableCharacter* Character = Cast<ACPP_PlayableCharacter>(GetOwner());
	ensure(Character != NULL);
	USkeletalMesh* WeaponMesh = LoadObject<USkeletalMesh>(NULL, *WeaponData->WeaponSkeletalMesh.ToSoftObjectPath().ToString());
	Character->GetTPWeaponComponent()->SetSkeletalMesh(WeaponMesh);
	//�� �׸���
	Character->GetShadowWeaponComponent()->SetSkeletalMesh(WeaponMesh);

	//źâ
	PerMagazineRounds = WeaponData->Rounds;
	RemainRounds = PerMagazineRounds * 5;
	CurrentMagazineRounds = PerMagazineRounds;

	Character->GetFPWeaponMeshComponent()->SetSkeletalMesh(WeaponMesh);
	Character->GetFPWeaponMeshComponent()->SetAnimInstanceClass(WeaponData->WeaponAnimClass);

	//���� �߻� ���
	CurrentWeaponMode = WeaponData->WeaponModes[0];

	// ���� �ݵ� ������
	WeaponRecoilCurve = LoadObject<UCurveVector>(NULL, *WeaponData->WeaponRecoilCurve.ToSoftObjectPath().ToString());

	//�ִϸ��̼�
	UAnimMontage* WeaponShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Weapon_Shoot.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(TEXT("Shoot"), WeaponShoot);
	UAnimMontage* HandsShoot = LoadObject<UAnimMontage>(NULL, *WeaponAnim->Hands_Shoot.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(TEXT("Shoot"), HandsShoot);

	LoadFPAnim(WeaponAnim->Weapon_Reload, WeaponAnim->Hands_Reload, TEXT("Reload"));
	LoadFPAnim(WeaponAnim->Weapon_TacReload, WeaponAnim->Hands_TacReload, TEXT("TacReload"));

	//�� ��ġ & ȸ��
	Character->SetIdleHandsLocation(WeaponData->IdleHandsLocation);
	Character->SetADSHandsLocation(WeaponData->ADSHandsLocation);
	Character->SetHandsRotation(WeaponData->HandsRotation);

	//�� �ִϸ��̼� �ʱ�ȭ
	UCPP_A_Hands* HandsAnimInstance = Cast<UCPP_A_Hands>(Character->GetHandsMeshComponent()->GetAnimInstance());
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_Idle.ToSoftObjectPath().ToString());
		HandsAnimInstance->IdleSequence = Anim;
	}
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_WalkForward.ToSoftObjectPath().ToString());
		HandsAnimInstance->MoveForwardSequence = Anim;
	}
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_WalkBackward.ToSoftObjectPath().ToString());
		HandsAnimInstance->MoveBackSequence = Anim;
	}
	{
		UAnimSequenceBase* Anim = LoadObject<UAnimSequenceBase>(NULL, *WeaponAnim->Hands_Sprint.ToSoftObjectPath().ToString());
		HandsAnimInstance->SprintSequence = Anim;
	}
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, CurrentMagazineRounds);
	DOREPLIFETIME(UWeaponComponent, RemainRounds);
}

void UWeaponComponent::LoadFPAnim(TSoftObjectPtr<UAnimMontage> _WeaponAnim, TSoftObjectPtr<UAnimMontage> _HandAnim, FName _Name)
{
	UAnimMontage* Weapon = LoadObject<UAnimMontage>(NULL, *_WeaponAnim.ToSoftObjectPath().ToString());
	FP_Weapon_MontageMap.Add(_Name, Weapon);
	UAnimMontage* Hands = LoadObject<UAnimMontage>(NULL, *_HandAnim.ToSoftObjectPath().ToString());
	FP_Hands_MontageMap.Add(_Name, Hands);
}

UAnimMontage* UWeaponComponent::Get_FP_Weapon_Montage(FName _Name) const
{
	UAnimMontage* ReturnValue = *FP_Weapon_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name�� �ش��ϴ� �ִϸ��̼��� Map�� �������� ����
	return ReturnValue;
}

UAnimMontage* UWeaponComponent::Get_FP_Hands_Montage(FName _Name) const
{
	UAnimMontage* ReturnValue = *FP_Hands_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name�� �ش��ϴ� �ִϸ��̼��� Map�� �������� ����
	return ReturnValue;
}

UAnimMontage* UWeaponComponent::Get_TP_Montage(FName _Name) const
{
	UAnimMontage* ReturnValue = *TP_MontageMap.Find(_Name);
	ensure(ReturnValue != NULL); // _Name�� �ش��ϴ� �ִϸ��̼��� Map�� �������� ����
	return ReturnValue;
}

class UDataTable* UWeaponComponent::GetWeaponDataTable() const
{
	check(WeaponDataTable != NULL)
	return WeaponDataTable;
}

class UDataTable* UWeaponComponent::GetWeaponAnimTable() const
{
	check(WeaponAnimTable != NULL)
	return WeaponAnimTable;
}

void UWeaponComponent::SetCurrentMagazineRounds(int _NewValue)
{
	if (GetOwner()->HasAuthority())
	{
		CurrentMagazineRounds = FMath::Clamp(_NewValue, 0, PerMagazineRounds + 1);
	}
}

void UWeaponComponent::SetRemainRounds(int _NewValue)
{
	if (GetOwner()->HasAuthority())
	{
		RemainRounds = FMath::Clamp(_NewValue, 0, 999);
	}
}

void UWeaponComponent::Reload_Inner()
{
	if (RemainRounds + CurrentMagazineRounds >= PerMagazineRounds)
	{
		if (CurrentMagazineRounds > 0)
		{
			RemainRounds += CurrentMagazineRounds;
			if (RemainRounds == PerMagazineRounds)
			{
				CurrentMagazineRounds = PerMagazineRounds;
			}
			else
			{
				CurrentMagazineRounds = PerMagazineRounds + 1;
			}
			RemainRounds -= CurrentMagazineRounds;
		}
		else
		{
			RemainRounds += CurrentMagazineRounds;
			CurrentMagazineRounds = PerMagazineRounds;
			RemainRounds -= CurrentMagazineRounds;
		}
	}
	else
	{
		RemainRounds += CurrentMagazineRounds;
		CurrentMagazineRounds = RemainRounds;
		RemainRounds = 0;
	}
}

void UWeaponComponent::Reload_RPC_Implementation()
{
	Reload_Inner();
}

