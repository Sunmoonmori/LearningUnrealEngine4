// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectileBounce.h"

AMyProjectileBounce::AMyProjectileBounce()
{
	if (CollisionComponent)
	{
		CollisionComponent->bReturnMaterialOnMove = true;
	}

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = 2000.0f;
		ProjectileMovementComponent->MaxSpeed = 2000.0f;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
		ProjectileMovementComponent->BounceVelocityStopSimulatingThreshold = 500.0f;
	}

	InitialLifeSpan = 5.0f;

	Damage = 60.f;
}
