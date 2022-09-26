// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "Character/MyProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerState.h"
#include "MyGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "AI/MyAICharacter.h"
#include "AI/MyAIController.h"
#include "Math/UnrealMathUtility.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BehaviorTree/BlackboardComponent.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/ThirdPersonCharacter"));
	if (ensure(PlayerPawnBPClass.Class != NULL))
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/ThirdPersonCPP/BP_MyPlayerController"));
	if (ensure(PlayerControllerBPClass.Class != NULL))
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	PlayerStateClass = AMyPlayerState::StaticClass();

	GameStateClass = AMyGameStateBase::StaticClass();

	static ConstructorHelpers::FClassFinder<ACharacter> TargetNPCBPClass(TEXT("/Game/AI/NPC/BP_NPCCharacter"));
	if (ensure(TargetNPCBPClass.Class != NULL))
	{
		TargetNPCClass = TargetNPCBPClass.Class;
	}

	bIsWin = true;
	GameOverTimeSecond = 60.f;

	EnemySpawnIntervalSecond = 5.0f;
	MaxEnemyNumber = 10;
	static ConstructorHelpers::FClassFinder<AMyAICharacter> EnemyBPClass_A(TEXT("/Game/AI/Enemy_A/BP_EnemyAICharacter_A"));
	if (ensure(EnemyBPClass_A.Class != NULL))
	{
		EnemyClasses.Add(EnemyBPClass_A.Class);
	}
	static ConstructorHelpers::FClassFinder<AMyAIController> EnemyControllerBPClass_A(TEXT("/Game/AI/Enemy_A/BP_EnemyAIController_A"));
	if (ensure(EnemyControllerBPClass_A.Class != NULL))
	{
		EnemyAIControllerClass.Add(EnemyControllerBPClass_A.Class);
	}
	static ConstructorHelpers::FClassFinder<AMyAICharacter> EnemyBPClass_B(TEXT("/Game/AI/Enemy_B/BP_EnemyAICharacter_B"));
	if (ensure(EnemyBPClass_B.Class != NULL))
	{
		EnemyClasses.Add(EnemyBPClass_B.Class);
	}
	static ConstructorHelpers::FClassFinder<AMyAIController> EnemyControllerBPClass_B(TEXT("/Game/AI/Enemy_B/BP_EnemyAIController_B"));
	if (ensure(EnemyControllerBPClass_B.Class != NULL))
	{
		EnemyAIControllerClass.Add(EnemyControllerBPClass_B.Class);
	}

	DefaultSlotName = TEXT("SlotName");
}

void AMyProjectGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

//	ReadSaveGame(DefaultSlotName);
}

void AMyProjectGameMode::InitGameState()
{
	Super::InitGameState();

	AMyGameStateBase* GS = Cast<AMyGameStateBase>(GameState);
	if (ensure(GS))
	{
		GS->GameOverTimeSecond = GameOverTimeSecond;
	}
}

void AMyProjectGameMode::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_GameOver, this, &AMyProjectGameMode::GameOver, GameOverTimeSecond, false);
	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawn, this, &AMyProjectGameMode::SpawnEnemy, EnemySpawnIntervalSecond, true);

	UWorld* World = GetWorld();
	if (ensure(World))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		TargetNPC = World->SpawnActor<ACharacter>(TargetNPCClass, NPCSpawnTransform, SpawnParams);
	}
}

void AMyProjectGameMode::GameOverLoseImmediately()
{
	bIsWin = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_GameOver);
	GameOver();
}

void AMyProjectGameMode::GameOver()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawn);
	ClearEnemy();

	AMyGameStateBase* GS = Cast<AMyGameStateBase>(GameState);
	if (ensure(GS))
	{
		if (bIsWin)
		{
			GS->bIsGameOverWin = true;
			GS->OnRep_GameOverWin();
		}
		else
		{
			GS->bIsGameOverLose = true;
			GS->OnRep_GameOverLose();
		}
	}
}

void AMyProjectGameMode::ClearEnemy()
{
	for (const auto e : EnemyAlive)
	{
		// XXX: put these in class AMyAIChatacter
		if (AMyGun* g = e->Gun)
		{
			g->Destroy();
		}
		if (AController* c = e->GetController())
		{
			c->Destroy();
		}
		e->Destroy();
	}
}

void AMyProjectGameMode::SpawnEnemy()
{
	if (EnemyAlive.Num() >= MaxEnemyNumber)
	{
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnEnemyQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AMyProjectGameMode::OnEnemySpawnQueryCompleted);
	}
}

void AMyProjectGameMode::OnEnemySpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	int32 RandomIndex = FMath::RandRange(0, EnemyClasses.Num() - 1);
	auto SpawnEnemyClass = EnemyClasses[RandomIndex];
	auto SpawnEnemyAIControllerClass = EnemyAIControllerClass[RandomIndex];

	if (Locations.IsValidIndex(0) && SpawnEnemyClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FRotator Rotation;

			AMyAIController* EnemyController = World->SpawnActor<AMyAIController>(SpawnEnemyAIControllerClass, Locations[0], Rotation, SpawnParams);
			AMyAICharacter* Enemy = World->SpawnActor<AMyAICharacter>(SpawnEnemyClass, Locations[0], Rotation, SpawnParams);

			EnemyController->Possess(Enemy);
			EnemyController->GetBlackboardComponent()->SetValueAsObject("TargetActor", TargetNPC); // set default attack target

			EnemyAlive.Add(Enemy);
		}
	}
}

void AMyProjectGameMode::RemoveEnemyFromRecord(AMyAICharacter* EnemyToBeRemoved)
{
	EnemyAlive.Remove(EnemyToBeRemoved);
}

void AMyProjectGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

//	// TODO: single player to multi player
//	AMyPlayerState* PS = NewPlayer->GetPlayerState<AMyPlayerState>();
//	if (PS)
//	{
//		PS->LoadFromMySaveGame(CurrentMySaveGame);
//	}
}

void AMyProjectGameMode::RespawnPlayer(APlayerController* PlayerController, FTransform& ReSpawnTransform)
{
	APawn* Pawn = PlayerController->GetPawn();
	if (Pawn)
	{
		Pawn->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UWorld* World = GetWorld();
	if (World)
	{
		APawn* NewPawn = World->SpawnActor<APawn>(DefaultPawnClass, ReSpawnTransform, SpawnParams);
		PlayerController->Possess(NewPawn);
	}
}

//void AMyProjectGameMode::WriteSaveGame(const FString& SlotName, const int32 UserIndex)
//{
//	// TODO: single player to multi player
//	AMyPlayerState* PS = Cast<AMyPlayerState>(GameState->PlayerArray[0]);
//	if (PS)
//	{
//		PS->SaveToMySaveGame(CurrentMySaveGame);
//	}
//
//	UGameplayStatics::SaveGameToSlot(CurrentMySaveGame, SlotName, UserIndex);
//}
//
//void AMyProjectGameMode::ReadSaveGame(const FString& SlotName, const int32 UserIndex)
//{
//	bool ShouldCreateNewMySaveGame = true;
//
//	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
//	{
//		CurrentMySaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
//		if (CurrentMySaveGame)
//		{
//			ShouldCreateNewMySaveGame = false;
//		}
//	}
//
//	if (ShouldCreateNewMySaveGame)
//	{
//		CurrentMySaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
//	}
//}
