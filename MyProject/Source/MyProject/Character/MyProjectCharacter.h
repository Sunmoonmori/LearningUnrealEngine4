// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Weapon/MyProjectile.h"
#include "MyCharacterAttributeComponent.h"
#include "../Weapon/MyGun.h"
#include "MyProjectCharacter.generated.h"

UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMyProjectCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	UFUNCTION()
	void UpdateAimingTargetLocation();

	UFUNCTION()
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION()
	void StopFiring();

	UFUNCTION(Server, Reliable)
	void ServerStopFiring();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Gun)
	AMyGun* Gun;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_GunChanged, Category = Gun)
	AMyGun* GunToBePickedUp;

	UFUNCTION(BlueprintCallable, Category = Gun)
	void PickUpGun();

	UFUNCTION()
	void DropGun();

	UFUNCTION()
	void OnRep_GunChanged();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UMyCharacterAttributeComponent* AttributeComp;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Gun)
	FVector AimingTargetLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Gun)
	bool bCanFire;

	UFUNCTION()
	void Interact();

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	UPROPERTY(ReplicatedUsing = OnRep_KilledBy)
	AActor* KilledBy;

	UFUNCTION()
	void OnRep_KilledBy();

	UFUNCTION()
	void Die(AActor* InstigatorActor);

	UFUNCTION(Server, Reliable)
	void ServerDie(AActor* InstigatorActor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = Respawn)
	float RespawnInterval;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	FTransform SpawnTransform;

	UFUNCTION()
	void CallRespawnPlayer();
};

