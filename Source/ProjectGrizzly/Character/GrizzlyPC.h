// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InventoryComponent.h"
#include "GrizzlyPC.generated.h"

/**
 *
 *
 */
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

	UInventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent;
	}
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = true))
	UInventoryComponent* InventoryComponent;
};
