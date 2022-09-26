// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverWin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverLose);

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AMyGameStateBase();

	// don't set, will set by GameMode
	// only used for UI
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated)
	float GameOverTimeSecond;

	UPROPERTY(ReplicatedUsing = OnRep_GameOverLose)
	bool bIsGameOverLose;
	
	UFUNCTION()
	void OnRep_GameOverLose();

	UPROPERTY(ReplicatedUsing = OnRep_GameOverWin)
	bool bIsGameOverWin;

	UFUNCTION()
	void OnRep_GameOverWin();

protected:
	UPROPERTY(BlueprintAssignable)
	FOnGameOverWin OnGameOverWin;
	
	UPROPERTY(BlueprintAssignable)
	FOnGameOverLose OnGameOverLose;
};
