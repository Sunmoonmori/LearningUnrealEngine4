// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "../Weapon/MyProjectile.h"
#include "MyBTTaskNode_Shoot.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBTTaskNode_Shoot : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = AI)
	TSubclassOf<AMyProjectile> ProjectileClass;
};
