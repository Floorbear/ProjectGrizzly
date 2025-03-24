// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerInventoryComponent.h"
#include "ProjectGrizzly/Core/GrizzlyGameInstance.h"
#include "GrizzlyPC.generated.h"

/**
 *
 *
 */
UENUM(BlueprintType)
enum class EInventoryCategory : uint8
{
	None,
	ItemContainer,
	Shop
};




USTRUCT(BlueprintType)
struct FInventoryUI_Parameter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInventoryCategory InventoryCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventoryComponent* Target;
};


UCLASS()
class PROJECTGRIZZLY_API AGrizzlyPC : public APlayerController
{
	GENERATED_BODY()
public:
	AGrizzlyPC();
protected:
	virtual void BeginPlay() override;
	//��Ʈ��ũ�� �����ϰ� ���͸� �ʱ�ȭ

	virtual void PostInitializeComponents() override;
	//����Ŭ�󿡼� ��Ʈ������ �ʱ�ȭ�� �ʿ��� ������ ���� �������� ��� ������ �� �� ȣ�� 'Ŭ���̾�Ʈ'������ ȣ��
	virtual void PostNetInit() override;

	virtual void OnPossess(APawn* aPawn) override;
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDeadNotificationUI();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OffDeadNotificationUI();

	UFUNCTION(BlueprintCallable)
	bool IsMyComputer();

	//--------------------------------------------------------------------------------------------------
	//										Inventory
	//--------------------------------------------------------------------------------------------------
private:
	UPROPERTY(meta = (AllowPrivateAccess = true), Category = "Inventory",EditAnywhere,BlueprintReadWrite)
	UUserWidget* InventoryUI = nullptr;
public:
	UFUNCTION(BlueprintCallable)
	UUserWidget* GetInventoryUI() const
	{
		return InventoryUI;
	}
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void ShowInventory(FInventoryUI_Parameter _Parameter);
	virtual void ShowInventory_Implementation(FInventoryUI_Parameter _Parameter){};
	
	UFUNCTION(BlueprintCallable)
	UPlayerInventoryComponent* GetInventoryComponent() const;

	//--------------------------------------------------------------------------------------------------
	//										Inventory - GameStart
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void LoadPlayerInventoryFromInstance();
	//--------------------------------------------------------------------------------------------------
	//										Network 
	//--------------------------------------------------------------------------------------------------
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//--------------------------------------------------------------------------------------------------
	//										Network - GameSet
	//--------------------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	EGamePhase GetGamePhase() const;

	//GameInstance is not replicated. So that Replicate GamePhase Parameter, Need to use Client RPC
	UFUNCTION(BlueprintCallable,Category = GamePhase, Client,Reliable)
	void SetGamePhase(EGamePhase _GamePhase);

	
	
};
