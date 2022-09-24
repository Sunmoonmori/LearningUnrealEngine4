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

// Sets default values
AMyAICharacter::AMyAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!PawnSensingComp)
	{
		PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
		PawnSensingComp->SetPeripheralVisionAngle(45.f);
	}

	BlackBoardTargetActorKey = "TargetActor";

	MaxHealth = 100.f;
	Health = MaxHealth;
}

void AMyAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AMyAICharacter::OnPawnSeen);
}

void AMyAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* MyController = Cast<AAIController>(GetController());
	if (MyController)
	{
		UBlackboardComponent* BlackBoardComp = MyController->GetBlackboardComponent();
		if (ensure(BlackBoardComp))
		{
			if (BlackBoardComp->GetValueAsObject(BlackBoardTargetActorKey) != Pawn)
			{
				BlackBoardComp->SetValueAsObject(BlackBoardTargetActorKey, Pawn);
			}
		}
	}
}

void AMyAICharacter::OnRep_KilledBy()
{
	if (KilledBy)
	{
		AAIController* MyController = Cast<AAIController>(GetController());
		if (MyController)
		{
			MyController->GetBrainComponent()->StopLogic("Killed");
		}

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();

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

	DOREPLIFETIME(AMyAICharacter, KilledBy);
}