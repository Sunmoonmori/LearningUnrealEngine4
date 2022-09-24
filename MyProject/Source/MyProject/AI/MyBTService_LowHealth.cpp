// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_LowHealth.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MyAICharacter.h"

void UMyBTService_LowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* MyController = OwnerComp.GetAIOwner();
	APawn* MyPawn = MyController->GetPawn();
	if (ensure(MyPawn))
	{
		AMyAICharacter* MyCharacter = Cast<AMyAICharacter>(MyPawn);
		if (MyCharacter)
		{
			bool bLowHealth = (MyCharacter->Health / MyCharacter->MaxHealth) < LowHealthFraction;

			UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
			BlackBoardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
		}
	}
}

UMyBTService_LowHealth::UMyBTService_LowHealth()
{
	LowHealthFraction = 0.3f;
}
