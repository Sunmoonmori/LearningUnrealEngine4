// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterAttributeComponent.h"
#include "MyProjectCharacter.h"
#include "Net/UnrealNetwork.h"


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

	SetIsReplicatedByDefault(true);
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
		ApplyMagicPointChange(GetOwner(), MagicPointRecoveredPerSecond * DeltaTime);
	}
}

bool UMyCharacterAttributeComponent::ApplyHitPointChange(AActor* InstigatorActor, float Delta)
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

	MulticastOnHitPointChanged(InstigatorActor, HitPoint, Delta);

	if (HitPoint <= 0.f && Delta < 0.f)
	{
		AMyProjectCharacter* Character = Cast<AMyProjectCharacter>(GetOwner());
		if (Character)
		{
			Character->Die(InstigatorActor);
		}
	}

	return true;
}

bool UMyCharacterAttributeComponent::ApplyMagicPointChange(AActor* InstigatorActor, float Delta)
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

	MulticastOnMagicPointChanged(InstigatorActor, MagicPoint, Delta);

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

void UMyCharacterAttributeComponent::MulticastOnHitPointChanged_Implementation(AActor* InstigatorActor, float NewHitPoint, float Delta)
{
	OnHitPointChanged.Broadcast(InstigatorActor, this, NewHitPoint, Delta);
}

void UMyCharacterAttributeComponent::MulticastOnMagicPointChanged_Implementation(AActor* InstigatorActor, float NewMagicPoint, float Delta)
{
	OnMagicPointChanged.Broadcast(InstigatorActor, this, NewMagicPoint, Delta);
}

void UMyCharacterAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyCharacterAttributeComponent, MaxHitPoint);
	DOREPLIFETIME(UMyCharacterAttributeComponent, MaxMagicPoint);
	DOREPLIFETIME(UMyCharacterAttributeComponent, HitPoint);
	DOREPLIFETIME(UMyCharacterAttributeComponent, MagicPoint);
}