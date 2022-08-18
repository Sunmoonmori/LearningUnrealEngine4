// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterAttributeComponent.h"


// Sets default values for this component's properties
UMyCharacterAttributeComponent::UMyCharacterAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	MaxHitPoint = 100.f;
	MaxMagicPoint = 100.f;
	HitPoint = MaxHitPoint;
	MagicPoint = MaxMagicPoint;
	MagicPointConsumedPerFiring = 13.f;
	MagicPointRecoveredPerSecond = 5.f;
}


// Called when the game starts
void UMyCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMyCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (MagicPoint < MaxMagicPoint)
	{
		ApplyMagicPointChange(MagicPointRecoveredPerSecond * DeltaTime);
	}
}

bool UMyCharacterAttributeComponent::ApplyHitPointChange(float Delta)
{
	// HitPoint can not be bigger than zero
	// HitPoint must be able to be less than zero

	// float can never be strictly equal to each other, but we still use beq here
	if (Delta > 0.f && HitPoint >= MaxHitPoint)
	{
		HitPoint = MaxHitPoint;
		return false;
	}

	HitPoint += Delta;

	if (HitPoint > MaxHitPoint) HitPoint = MaxHitPoint;

	OnHitPointChanged.Broadcast(nullptr, this, HitPoint, Delta);

	return true;
}

bool UMyCharacterAttributeComponent::ApplyMagicPointChange(float Delta)
{
	// MagicPoint can not be bigger than zero
	// MagicPoint must be able to be less than zero

	// float can never be strictly equal to each other, but we still use beq here
	if (Delta > 0.f && MagicPoint >= MaxMagicPoint)
	{
		MagicPoint = MaxMagicPoint;
		return false;
	}

	MagicPoint += Delta;

	if (MagicPoint > MaxMagicPoint) MagicPoint = MaxMagicPoint;

	OnMagicPointChanged.Broadcast(nullptr, this, MagicPoint, Delta);

	return true;
}

float UMyCharacterAttributeComponent::GetHitPoint()
{
	return HitPoint;
}

float UMyCharacterAttributeComponent::GetMagicPoint()
{
	return MagicPoint;
}

// TODO: can be more complicated and can have more input parameters
float UMyCharacterAttributeComponent::GetMagicPointConsumed()
{
	return MagicPointConsumedPerFiring;
}
