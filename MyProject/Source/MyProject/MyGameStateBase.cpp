// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameStateBase.h"
#include "Net/UnrealNetwork.h"

AMyGameStateBase::AMyGameStateBase()
{
	bIsGameOverLose = false;
	bIsGameOverWin = false;
}

void AMyGameStateBase::OnRep_GameOverWin()
{
	OnGameOverWin.Broadcast();
}

void AMyGameStateBase::OnRep_GameOverLose()
{
	OnGameOverLose .Broadcast();
}

void AMyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameStateBase, bIsGameOverWin);
	DOREPLIFETIME(AMyGameStateBase, bIsGameOverLose);
}
