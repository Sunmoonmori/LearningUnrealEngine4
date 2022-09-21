// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyCharacterAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHitPointChanged, AActor*, InstigatorActor, UMyCharacterAttributeComponent*, OwningComp, float, NewHitPoint, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMagicPointChanged, AActor*, InstigatorActor, UMyCharacterAttributeComponent*, OwningComp, float, NewMagicPoint, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyCharacterAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Attribute)
	float MaxHitPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Attribute)
	float MaxMagicPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Attribute)
	float HitPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Attribute)
	float MagicPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attribute)
	float MagicPointConsumedPerFiring;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attribute)
	float MagicPointRecoveredPerSecond;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHitPointChanged(AActor* InstigatorActor, float NewHitPoint, float Delta);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMagicPointChanged(AActor* InstigatorActor, float NewMagicPoint, float Delta);

	UPROPERTY(BlueprintAssignable)
	FOnHitPointChanged OnHitPointChanged;

	UPROPERTY(BlueprintAssignable)
	FOnMagicPointChanged OnMagicPointChanged;

public:
	// Should Be Executed By Server Functions Only
	UFUNCTION(BlueprintCallable, Category = Attribute)
	bool ApplyHitPointChange(AActor* InstigatorActor, float Delta);

	// Should Be Executed By Server Functions Only
	UFUNCTION(BlueprintCallable, Category = Attribute)
	bool ApplyMagicPointChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = Attribute)
	float GetHitPoint();

	UFUNCTION(BlueprintCallable, Category = Attribute)
	float GetMagicPoint();

	UFUNCTION(BlueprintCallable, Category = Attribute)
	float GetMagicPointConsumed();
};
