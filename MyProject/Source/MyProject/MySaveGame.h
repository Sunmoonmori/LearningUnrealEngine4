// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FNameScorePair
{
	GENERATED_BODY()

public:
	FNameScorePair();

	FNameScorePair(FString& PlayerNickName, int32 PlayerMyScore);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	FString PlayerNickName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	int32 PlayerMyScore;
};


/**
 * 
 */
UCLASS()
class MYPROJECT_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UMySaveGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	FString DefaultPlayerNickName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	int32 DefaultMyScore;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	int32 ScoreRecordNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	TArray<FNameScorePair> ScoreRecord; // should not used as heap

};
