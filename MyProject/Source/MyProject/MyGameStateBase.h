// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AMyGameStateBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CountDown)
	float GameOverTimeSecond;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_GameOver, Category = GameOver)
	bool bIsGameOver;

	UFUNCTION()
	void OnRep_GameOver();

protected:
	UPROPERTY(BlueprintAssignable)
	FOnGameOver OnGameOver;
};
