// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "MyGun.generated.h"

UCLASS()
class MYPROJECT_API AMyGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnGunPickedUp();

	UFUNCTION()
	void OnGunDropped();

	UFUNCTION()
	void StartFire(AActor* FireInstigator);

	UFUNCTION()
	void StopFire();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Interaction)
	USphereComponent* OverlapComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMyProjectile> ProjectileClass;

	UPROPERTY()
	bool bIsFiring;
};
