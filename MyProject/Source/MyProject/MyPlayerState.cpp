// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Misc/DateTime.h"

AMyPlayerState::AMyPlayerState()
{
	KillCount = 0;
	DeathCount = 0;
	NickName = FString(TEXT("DefaultName-")) + FString::FromInt(FDateTime::Now().ToUnixTimestamp());
}

void AMyPlayerState::AddKill()
{
	KillCount++;
	MulticastOnKillChanged(KillCount, 1);
}

void AMyPlayerState::ClearKill()
{
	int32 Delta = -KillCount;
	KillCount = 0;
	MulticastOnKillChanged(KillCount, Delta);
}

int32 AMyPlayerState::GetKill()
{
	return KillCount;
}

void AMyPlayerState::AddDeath()
{
	DeathCount++;
	MulticastOnDeathChanged(DeathCount, 1);
}

void AMyPlayerState::ClearDeath()
{
	int32 Delta = -DeathCount;
	DeathCount = 0;
	MulticastOnDeathChanged(DeathCount, Delta);
}

int32 AMyPlayerState::GetDeath()
{
	return DeathCount;
}

void AMyPlayerState::MulticastOnKillChanged_Implementation(int32 NewKillCount, int32 Delta)
{
	OnKillChanged.Broadcast(this, NewKillCount, Delta);
}

void AMyPlayerState::MulticastOnDeathChanged_Implementation(int32 NewDeathCount, int32 Delta)
{
	OnDeathChanged.Broadcast(this, NewDeathCount, Delta);
}

FString AMyPlayerState::GetNickName()
{
	return NickName;
}

void AMyPlayerState::SetNickName(const FString& NewNickName)
{
	NickName = NewNickName;
}

//int32 AMyPlayerState::GetCurrentScoreRecordIndex()
//{
//	int32 TargetIndex = ScoreRecordNum;
//	for (int32 i = 0; i < ScoreRecordNum; i++)
//	{
//		if (MyScore >= ScoreRecord[i].PlayerMyScore)
//		{
//			TargetIndex = i;
//			break;
//		}
//	}
//
//	return TargetIndex;
//}
//
//void AMyPlayerState::UpdateMyPlayerStateScoreRecord(int32 TargetIndex)
//{
//	if (TargetIndex >= ScoreRecordNum) return;
//
//	for (int32 i = ScoreRecordNum - 1; i > TargetIndex; i--)
//	{
//		ScoreRecord.Swap(i, i - 1);
//	}
//
//	ScoreRecord[TargetIndex] = FNameScorePair(NickName, MyScore);
//}
//
//void AMyPlayerState::LoadFromMySaveGame(const UMySaveGame* CurrentMySaveGame)
//{
//	ScoreRecordNum = CurrentMySaveGame->ScoreRecordNum;
//	ScoreRecord = CurrentMySaveGame->ScoreRecord;
//}
//
//void AMyPlayerState::SaveToMySaveGame(UMySaveGame* CurrentMySaveGame)
//{
//	UpdateMyPlayerStateScoreRecord(GetCurrentScoreRecordIndex());
//	CurrentMySaveGame->ScoreRecordNum = ScoreRecordNum;
//	CurrentMySaveGame->ScoreRecord = ScoreRecord;
//}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, KillCount);
	DOREPLIFETIME(AMyPlayerState, DeathCount);
	DOREPLIFETIME(AMyPlayerState, NickName);
}