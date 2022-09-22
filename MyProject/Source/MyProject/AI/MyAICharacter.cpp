// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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
