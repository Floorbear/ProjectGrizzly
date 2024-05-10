// Fill out your copyright notice in the Description page of Project Settings.


#include "GrizzlyPC.h"

bool AGrizzlyPC::IsMyComputer()
{
	if (GetNetMode() == NM_Standalone)
	{
		return true;
	}
	else if (GetNetMode() == NM_Client)
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			return true;
		}
	}
	return false;
}
