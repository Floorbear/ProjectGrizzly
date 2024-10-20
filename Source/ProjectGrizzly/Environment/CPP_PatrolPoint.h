// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine\DataTable.h>
#include "CPP_PatrolPoint.generated.h"

UENUM(BlueprintType)
enum class EPatrolAction : uint8
{
	None				UMETA(DisplayName = "None"), //�����ϸ� �ٷ� ���� ����Ʈ�� �̵�
	Wait				UMETA(DisplayName = "Wait"), //�����ϸ� �����ð� ��� �� �̵�
	Rest			UMETA(DisplayName = "Rest")

};

UCLASS()
class PROJECTGRIZZLY_API ACPP_PatrolPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACPP_PatrolPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPatrolAction PatrolAction;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;


};


USTRUCT(BlueprintType)
struct FPatrolRoute : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<ACPP_PatrolPoint>> Route;

};
