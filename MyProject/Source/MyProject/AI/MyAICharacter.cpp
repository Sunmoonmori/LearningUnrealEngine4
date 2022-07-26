// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/MyProjectCharacter.h"
#include "../MyPlayerState.h"
#include "../MyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "../MyProjectGameMode.h"
#include "../Weapon/MyGun.h"

// Sets default values
AMyAICharacter::AMyAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// if (!PawnSensingComp)
	// {
	// 	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	// 	PawnSensingComp->SetPeripheralVisionAngle(45.f);
	// }

	// BlackBoardTargetActorKey = "TargetActor";

	MaxHealth = 100.f;
	Health = MaxHealth;
}

void AMyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FTransform GunTransform = GetMesh()->GetSocketTransform(TEXT("RifleHoldSocket"));
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Gun = GetWorld()->SpawnActor<AMyGun>(GunClass, GunTransform, Params);
		OnRep_AttachGun();
	}
}

void AMyAICharacter::OnRep_AttachGun()
{
	if (Gun)
	{
		Gun->OnGunPickedUp();
		Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RifleHoldSocket"));
	}
}

// void AMyAICharacter::PostInitializeComponents()
// {
// 	Super::PostInitializeComponents();
// 
// 	PawnSensingComp->OnSeePawn.AddDynamic(this, &AMyAICharacter::OnPawnSeen);
// }

// void AMyAICharacter::OnPawnSeen(APawn* Pawn)
// {
// 	// already focus on TargetNPC before being attacked
// 	if (Health >= MaxHealth)
// 	{
// 		return;
// 	}
// 
// 	AAIController* MyController = Cast<AAIController>(GetController());
// 	if (MyController)
// 	{
// 		UBlackboardComponent* BlackBoardComp = MyController->GetBlackboardComponent();
// 		if (ensure(BlackBoardComp))
// 		{
// 			if (BlackBoardComp->GetValueAsObject(BlackBoardTargetActorKey) != Pawn)
// 			{
// 				BlackBoardComp->SetValueAsObject(BlackBoardTargetActorKey, Pawn);
// 			}
// 		}
// 	}
// }

void AMyAICharacter::OnRep_KilledBy()
{
	if (KilledBy)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (Gun)
		{
			Gun->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
			Gun->OnGunDropped();
		}

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");

		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);

		SetLifeSpan(5.0f);
	}
}

void AMyAICharacter::Die(AController* InstigatorController)
{
	if (HasAuthority())
	{
		if (!KilledBy) {
			AMyPlayerController* InstigatorMyPlayerController = Cast<AMyPlayerController>(InstigatorController);
			if (InstigatorMyPlayerController)
			{
				InstigatorMyPlayerController->GetPlayerState<AMyPlayerState>()->AddKill();
			}

			AAIController* MyController = Cast<AAIController>(GetController());
			if (MyController)
			{
				MyController->GetBrainComponent()->StopLogic("Killed");
				MyController->Destroy();
			}

			UWorld* World = GetWorld();
			if (World)
			{
				AMyProjectGameMode* GM = Cast<AMyProjectGameMode>(World->GetAuthGameMode());
				if (GM)
				{
					GM->RemoveEnemyFromRecord(this);
				}
			}

			KilledBy = InstigatorMyPlayerController;
			OnRep_KilledBy();
		}
	}
	else
	{
		ServerDie(InstigatorController);
	}
}

void AMyAICharacter::ServerDie_Implementation(AController* InstigatorController)
{
	Die(InstigatorController);
}

void AMyAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyAICharacter, Gun);
	DOREPLIFETIME(AMyAICharacter, KilledBy);
	DOREPLIFETIME(AMyAICharacter, Health);
	DOREPLIFETIME(AMyAICharacter, MaxHealth);
}