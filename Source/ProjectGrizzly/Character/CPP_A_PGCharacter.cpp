// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_A_PGCharacter.h"

void UCPP_A_PGCharacter::AnimNotify_OnReloadStart()
{
	if (OnReloadStartDelegate.IsBound())
	{
		OnReloadStartDelegate.Broadcast();
	}
}
