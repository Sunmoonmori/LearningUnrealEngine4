// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MySaveGame.h"
#include "MyProjectGameMode.generated.h"

UCLASS(minimalapi)
class AMyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProjectGameMode();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SaveGame)
	FString DefaultSlotName;

	UPROPERTY()
	UMySaveGame* CurrentMySaveGame;

public:
	UPROPERTY()
	TSubclassOf<class AMyProjectCharacter> CharacterClass;

	UPROPERTY()
	FTransform CharacterSpawnTransform;

	UFUNCTION()
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION()
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category = SaveGame)
	void WriteSaveGame(const FString& SlotName, const int32 UserIndex=0);

	UFUNCTION()
	void ReadSaveGame(const FString& SlotName, const int32 UserIndex=0);

	UFUNCTION()
	void RespawnCharacter(APlayerController* PC);
};



