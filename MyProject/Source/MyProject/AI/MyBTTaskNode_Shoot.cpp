// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTaskNode_Shoot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/MyProjectCharacter.h"

EBTNodeResult::Type UMyBTTaskNode_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		AMyProjectCharacter* TargetCharacter = Cast<AMyProjectCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (TargetCharacter == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		if (TargetCharacter->AttributeComp->GetHitPoint() <= 0.f)
		{
			return EBTNodeResult::Failed;
		}

		FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_tmp");
		FVector Direction = TargetCharacter->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = Direction.Rotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyPawn;

		AMyProjectile* Projectile = GetWorld()->SpawnActor<AMyProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);
		if (Projectile)
		{
			FVector LaunchDirection = MuzzleRotation.Vector();
			Projectile->FireInDirection(LaunchDirection);

			return EBTNodeResult::Succeeded;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}

	return EBTNodeResult::Failed;
}
