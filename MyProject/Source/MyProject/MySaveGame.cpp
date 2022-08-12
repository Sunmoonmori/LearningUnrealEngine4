// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGame.h"


FNameScorePair::FNameScorePair()
{
	PlayerNickName = FString();
	PlayerMyScore = 0;
}

FNameScorePair::FNameScorePair(FString& PlayerNickName, int32 PlayerMyScore)
{
	this->PlayerNickName = PlayerNickName;
	this->PlayerMyScore = PlayerMyScore;
}

UMySaveGame::UMySaveGame()
{
	DefaultPlayerNickName = TEXT("-------");
	DefaultMyScore = 0;
	ScoreRecordNum = 5;
	ScoreRecord.SetNum(ScoreRecordNum);
	for (int32 i = 0; i < ScoreRecordNum; i++)
	{
		ScoreRecord[i] = FNameScorePair(DefaultPlayerNickName, DefaultMyScore);
	}
}
