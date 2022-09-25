// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAICharacter.generated.h"

class UPawnSensingComponent;

UCLASS()
class MYPROJECT_API AMyAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyAICharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = Attribute)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = Attribute)
	float MaxHealth;

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName BlackBoardTargetActorKey;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UPROPERTY(ReplicatedUsing = OnRep_KilledBy)
	AActor* KilledBy;

	UFUNCTION()
	void OnRep_KilledBy();

	UFUNCTION(BlueprintCallable)
	void Die(AController* InstigatorController);

	UFUNCTION(Server, Reliable)
	void ServerDie(AController* InstigatorController);

};
