// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MyBTService_LowHealth.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBTService_LowHealth : public UBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = AI)
	FBlackboardKeySelector LowHealthKey;

	UPROPERTY()
	float LowHealthFraction;

public:
	UMyBTService_LowHealth();
};
