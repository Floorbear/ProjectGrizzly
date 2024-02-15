// Fill out your copyright notice in the Description page of Project Settings.


#include "TickTask.h"

UTickTask::UTickTask()
{
	bTickingTask = true;
}

UTickTask* UTickTask::CreateTickTask(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UTickTask* MyObj = NewAbilityTask<UTickTask>(OwningAbility);
	return MyObj;
}

void UTickTask::Activate()
{
	Super::Activate();
}

void UTickTask::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
