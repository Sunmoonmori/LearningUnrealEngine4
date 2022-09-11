// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "MySaveGame.h"
#include "MyProjectGameMode.generated.h"

UCLASS(minimalapi)
class AMyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProjectGameMode();

protected:
	UFUNCTION()
	void GameOver();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SaveGame)
	FString DefaultSlotName;

//	UPROPERTY()
//	UMySaveGame* CurrentMySaveGame;

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// XXX: Should ReSpawnTransform use reference or not?
	UFUNCTION()
	void RespawnPlayer(APlayerController* PlayerController, FTransform& ReSpawnTransform);

//	UFUNCTION(BlueprintCallable, Category = SaveGame)
//	void WriteSaveGame(const FString& SlotName, const int32 UserIndex=0);
//
//	UFUNCTION()
//	void ReadSaveGame(const FString& SlotName, const int32 UserIndex=0);
};



