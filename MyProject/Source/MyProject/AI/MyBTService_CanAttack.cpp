// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_CanAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void UMyBTService_CanAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));
		if (TargetActor)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();

			APawn* MyPawn = MyController->GetPawn();
			if (ensure(MyPawn))
			{
				float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), MyPawn->GetActorLocation());

				bool bWithinRange = DistanceTo < MaxAttackRange;

				bool bCanSee = false;
				if (bWithinRange)
				{
					bCanSee = MyController->LineOfSightTo(TargetActor);
				}

				BlackBoardComp->SetValueAsBool(CanAttackKey.SelectedKeyName, (bWithinRange && bCanSee));
			}
		}
	}
}

UMyBTService_CanAttack::UMyBTService_CanAttack()
{
	MaxAttackRange = 2000.f;
}
