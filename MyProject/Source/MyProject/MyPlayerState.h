// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MySaveGame.h"
#include "MyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMyScoreChanged, APlayerState*, PlayerState, int32, NewMyScore, int32, Delta);

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = MyScore)
	int32 MyScore; // self defined score instead of Score in APlayerState

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = MyScore)
	FString NickName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	int32 ScoreRecordNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
	TArray<FNameScorePair> ScoreRecord;

public:
	AMyPlayerState();

	UFUNCTION(BlueprintCallable, Category = MyScore)
	bool ApplyMyScoreChange(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = MyScore)
	int32 GetMyScore();

	UFUNCTION(BlueprintCallable, Category = MyScore)
	bool SetMyScore(int32 NewMyScore);

	UPROPERTY(BlueprintAssignable)
	FOnMyScoreChanged OnMyScoreChanged;

	UFUNCTION(BlueprintCallable, Category = MyScore)
	FString GetNickName();

	UFUNCTION(BlueprintCallable, Category = MyScore)
	bool SetNickName(const FString& NewNickName);

	// current index of this playerstate in score record sheet of this playerstate
	UFUNCTION(BlueprintCallable, Category = ScoreRecord)
	int32 GetCurrentScoreRecordIndex();

	UFUNCTION(BlueprintCallable, Category = ScoreRecord)
	void UpdateMyPlayerStateScoreRecord(int32 TargetIndex);

	UFUNCTION(BlueprintCallable, Category = SaveGame)
	void LoadFromMySaveGame(const UMySaveGame* CurrentMySaveGame);

	UFUNCTION(BlueprintCallable, Category = SaveGame)
	void SaveToMySaveGame(UMySaveGame* CurrentMySaveGame);
};
