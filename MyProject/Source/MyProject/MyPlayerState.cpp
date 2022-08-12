// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

AMyPlayerState::AMyPlayerState()
{
	MyScore = 0;
	NickName = TEXT("DefaultName");
}

bool AMyPlayerState::ApplyMyScoreChange(int32 Delta)
{
	MyScore += Delta;

	OnMyScoreChanged.Broadcast(this, MyScore, Delta);

	return true;
}

int32 AMyPlayerState::GetMyScore()
{
	return MyScore;
}

bool AMyPlayerState::SetMyScore(int32 NewMyScore)
{
	int32 Delta = NewMyScore - MyScore;
	MyScore = NewMyScore;

	OnMyScoreChanged.Broadcast(this, MyScore, Delta);

	return true;
}

FString AMyPlayerState::GetNickName()
{
	return NickName;
}

bool AMyPlayerState::SetNickName(const FString& NewNickName)
{
	NickName = NewNickName;

	return true;
}

int32 AMyPlayerState::GetCurrentScoreRecordIndex()
{
	int32 TargetIndex = ScoreRecordNum;
	for (int32 i = 0; i < ScoreRecordNum; i++)
	{
		if (MyScore >= ScoreRecord[i].PlayerMyScore)
		{
			TargetIndex = i;
			break;
		}
	}

	return TargetIndex;
}

void AMyPlayerState::UpdateMyPlayerStateScoreRecord(int32 TargetIndex)
{
	if (TargetIndex >= ScoreRecordNum) return;

	for (int32 i = ScoreRecordNum - 1; i > TargetIndex; i--)
	{
		ScoreRecord.Swap(i, i - 1);
	}

	ScoreRecord[TargetIndex] = FNameScorePair(NickName, MyScore);
}

void AMyPlayerState::LoadFromMySaveGame(const UMySaveGame* CurrentMySaveGame)
{
	ScoreRecordNum = CurrentMySaveGame->ScoreRecordNum;
	ScoreRecord = CurrentMySaveGame->ScoreRecord;
}

void AMyPlayerState::SaveToMySaveGame(UMySaveGame* CurrentMySaveGame)
{
	UpdateMyPlayerStateScoreRecord(GetCurrentScoreRecordIndex());
	CurrentMySaveGame->ScoreRecordNum = ScoreRecordNum;
	CurrentMySaveGame->ScoreRecord = ScoreRecord;
}
