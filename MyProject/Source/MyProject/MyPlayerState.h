// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
//#include "MySaveGame.h"
#include "MyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKillChanged, AMyPlayerState*, PlayerState, int32, NewKillCount, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeathChanged, AMyPlayerState*, PlayerState, int32, NewDeathCount, int32, Delta);

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = MyScore)
	int32 KillCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = MyScore)
	int32 DeathCount;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnKillChanged(int32 NewKillCount, int32 Delta);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnDeathChanged(int32 NewDeathCount, int32 Delta);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = MyScore)
	FString NickName;

	UPROPERTY(BlueprintAssignable)
	FOnKillChanged OnKillChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDeathChanged OnDeathChanged;

//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
//	int32 ScoreRecordNum;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ScoreRecord)
//	TArray<FNameScorePair> ScoreRecord;

public:
	AMyPlayerState();

	// Should Be Executed By Server Functions Only
	UFUNCTION(BlueprintCallable, Category = MyScore)
	void AddKill();

	// Should Be Executed By Server Functions Only
	UFUNCTION(BlueprintCallable, Category = MyScore)
	void ClearKill();

	UFUNCTION(BlueprintCallable, Category = MyScore)
	int32 GetKill();

	// Should Be Executed By Server Functions Only
	UFUNCTION(BlueprintCallable, Category = MyScore)
	void AddDeath();

	// Should Be Executed By Server Functions Only
	UFUNCTION(BlueprintCallable, Category = MyScore)
	void ClearDeath();

	UFUNCTION(BlueprintCallable, Category = MyScore)
	int32 GetDeath();

	UFUNCTION(BlueprintCallable, Category = MyScore)
	FString GetNickName();

	// Should Be Executed By Server Functions Only
	UFUNCTION(BlueprintCallable, Category = MyScore)
	void SetNickName(const FString& NewNickName);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = MyScore)
	void ServerSetNickName(const FString& NewNickName);

//	// current index of this playerstate in score record sheet of this playerstate
//	UFUNCTION(BlueprintCallable, Category = ScoreRecord)
//	int32 GetCurrentScoreRecordIndex();
//
//	UFUNCTION(BlueprintCallable, Category = ScoreRecord)
//	void UpdateMyPlayerStateScoreRecord(int32 TargetIndex);
//
//	UFUNCTION(BlueprintCallable, Category = SaveGame)
//	void LoadFromMySaveGame(const UMySaveGame* CurrentMySaveGame);
//
//	UFUNCTION(BlueprintCallable, Category = SaveGame)
//	void SaveToMySaveGame(UMySaveGame* CurrentMySaveGame);
};
