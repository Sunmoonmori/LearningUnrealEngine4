// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "MyProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/BP_MyPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	PlayerStateClass = AMyPlayerState::StaticClass();

	DefaultSlotName = TEXT("SlotName");
}

void AMyProjectGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	ReadSaveGame(DefaultSlotName);
}

void AMyProjectGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// TODO: single player to multi player
	AMyPlayerState* PS = NewPlayer->GetPlayerState<AMyPlayerState>();
	if (PS)
	{
		PS->LoadFromMySaveGame(CurrentMySaveGame);
	}
}

void AMyProjectGameMode::WriteSaveGame(const FString& SlotName, const int32 UserIndex)
{
	// TODO: single player to multi player
	AMyPlayerState* PS = Cast<AMyPlayerState>(GameState->PlayerArray[0]);
	if (PS)
	{
		PS->SaveToMySaveGame(CurrentMySaveGame);
	}

	UGameplayStatics::SaveGameToSlot(CurrentMySaveGame, SlotName, UserIndex);
}

void AMyProjectGameMode::ReadSaveGame(const FString& SlotName, const int32 UserIndex)
{
	bool ShouldCreateNewMySaveGame = true;

	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		CurrentMySaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		if (CurrentMySaveGame)
		{
			ShouldCreateNewMySaveGame = false;
		}
	}

	if (ShouldCreateNewMySaveGame)
	{
		CurrentMySaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	}
}

void AMyProjectGameMode::RespawnCharacter(APlayerController* PC)
{
	APawn* ControlledPawn = PC->GetPawn();
	if (ControlledPawn)
	{
		ControlledPawn->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMyProjectCharacter* Character = GetWorld()->SpawnActor<AMyProjectCharacter>(CharacterClass, CharacterSpawnTransform, SpawnParams);
	if (Character)
	{
		PC->Possess(Character);
	}
}