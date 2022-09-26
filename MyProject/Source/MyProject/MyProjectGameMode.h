// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "MySaveGame.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "MyProjectGameMode.generated.h"

class AMyAICharacter;
class AMyAIController;
class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;

UCLASS(minimalapi)
class AMyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProjectGameMode();

protected:
	UPROPERTY()
	TSubclassOf<ACharacter> TargetNPCClass;

	UPROPERTY()
	ACharacter* TargetNPC;

	UPROPERTY(EditDefaultsOnly, Category = Target)
	FTransform NPCSpawnTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CountDown)
	float GameOverTimeSecond;

	UPROPERTY()
	FTimerHandle TimerHandle_GameOver;

	// used to judge win or lose when game over
	UPROPERTY()
	bool bIsWin;

	UFUNCTION(BlueprintCallable)
	void GameOverLoseImmediately();

	UFUNCTION()
	void GameOver();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
	float EnemySpawnIntervalSecond;
	
	UPROPERTY()
	TArray<TSubclassOf<AMyAICharacter>> EnemyClasses;
	
	UPROPERTY()
	TArray<TSubclassOf<AMyAIController>> EnemyAIControllerClass;

	UPROPERTY()
	TSet<AMyAICharacter*> EnemyAlive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
	int32 MaxEnemyNumber;

	UPROPERTY()
	FTimerHandle TimerHandle_EnemySpawn;
	
	UPROPERTY(EditDefaultsOnly, Category = Enemy)
	UEnvQuery* SpawnEnemyQuery;

	UFUNCTION()
	void SpawnEnemy();

	UFUNCTION()
	void OnEnemySpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	UFUNCTION()
	void ClearEnemy();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SaveGame)
	FString DefaultSlotName;

//	UPROPERTY()
//	UMySaveGame* CurrentMySaveGame;

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() override;

	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// XXX: Should ReSpawnTransform use reference or not?
	UFUNCTION()
	void RespawnPlayer(APlayerController* PlayerController, FTransform& ReSpawnTransform);

	UFUNCTION()
	void RemoveEnemyFromRecord(AMyAICharacter* EnemyToBeRemoved);

//	UFUNCTION(BlueprintCallable, Category = SaveGame)
//	void WriteSaveGame(const FString& SlotName, const int32 UserIndex=0);
//
//	UFUNCTION()
//	void ReadSaveGame(const FString& SlotName, const int32 UserIndex=0);
};



