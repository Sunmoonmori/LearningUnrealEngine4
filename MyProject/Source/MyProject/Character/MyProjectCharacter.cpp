// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../MyProjectGameMode.h"

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 90.f, 60.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	AttributeComp = CreateDefaultSubobject<UMyCharacterAttributeComponent>(TEXT("AttributeComp"));

	bCanFire = true;
	Gun = nullptr;

	KilledBy = nullptr;

	RespawnInterval = 5.f;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyProjectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyProjectCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyProjectCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyProjectCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMyProjectCharacter::StopFiring);

	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMyProjectCharacter::Interact);
}


void AMyProjectCharacter::OnResetVR()
{
	// If MyProject is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MyProject.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyProjectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMyProjectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMyProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

//////////////////////////////////////////////////////////////////////////
// Override

void AMyProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();
}

void AMyProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAimingTargetLocation();
}

void AMyProjectCharacter::Destroyed()
{
	Super::Destroyed();
}

//////////////////////////////////////////////////////////////////////////
// AimingTarget

void AMyProjectCharacter::UpdateAimingTargetLocation()
{
	UWorld* World = GetWorld();
	if (World)
	{
		const float LineTraceDistanceFallback = 10000.f;

		FVector TraceDirection = FollowCamera->GetComponentRotation().Vector();
		FVector TraceStart = FollowCamera->GetComponentLocation();
		FVector TraceEnd = TraceStart + (TraceDirection * LineTraceDistanceFallback);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		FHitResult OutHit;
		bool isHit = World->LineTraceSingleByProfile(OutHit, TraceStart, TraceEnd, TEXT("MyProjectile"), QueryParams);
		if (isHit) 
		{
			AimingTargetLocation = OutHit.ImpactPoint;
		}
		else
		{
			AimingTargetLocation = TraceEnd;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Action

void AMyProjectCharacter::Fire()
{
	if (HasAuthority())
	{
		if (bCanFire && Gun)
		{
			Gun->StartFire(this);
		}
	}
	else
	{
		ServerFire();
	}
}

void AMyProjectCharacter::ServerFire_Implementation()
{
	Fire();
}

void AMyProjectCharacter::StopFiring()
{
	if (HasAuthority())
	{
		if (Gun)
		{
			Gun->StopFire();
		}
	}
	else
	{
		ServerStopFiring();
	}
}

void AMyProjectCharacter::ServerStopFiring_Implementation()
{
	StopFiring();
}

void AMyProjectCharacter::PickUpGun()
{
	if (GunToBePickedUp)
	{
		if (Gun)
		{
			DropGun();
		}
		GunToBePickedUp->OnGunPickedUp();
		GunToBePickedUp->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RifleHoldSocket"));
		Gun = GunToBePickedUp;
	}
}

void AMyProjectCharacter::DropGun()
{
	if (Gun)
	{
		Gun->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
		Gun->OnGunDropped();
		Gun = nullptr;
	}
}

void AMyProjectCharacter::OnRep_GunChanged()
{
	PickUpGun();
}

void AMyProjectCharacter::Interact()
{
	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		GetCapsuleComponent()->GetOverlappingActors(OverlappingActors);
		for (AActor* a : OverlappingActors)
		{
			AMyGun* MyGunActor = Cast<AMyGun>(a);
			if (MyGunActor)
			{
				GunToBePickedUp = MyGunActor;
				OnRep_GunChanged();
				break;
			}
		}
	}
	else
	{
		ServerInteract();
	}

}

void AMyProjectCharacter::ServerInteract_Implementation()
{
	Interact();
}

void AMyProjectCharacter::OnRep_KilledBy()
{
	if (KilledBy)
	{
		DisableInput(nullptr); // TODO: only disable some of the input
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DropGun();
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);
	}
}

void AMyProjectCharacter::Die(AActor* InstigatorActor)
{
	if (HasAuthority())
	{
		if (!KilledBy) {
			AMyProjectCharacter* InstigatorCharacter = Cast<AMyProjectCharacter>(InstigatorActor);
			if (InstigatorCharacter)
			{
				InstigatorCharacter->GetPlayerState<AMyPlayerState>()->AddKill();
			}
			GetPlayerState<AMyPlayerState>()->AddDeath();

			KilledBy = InstigatorActor;
			OnRep_KilledBy();

			FTimerHandle MemberTimerHandle;
			GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMyProjectCharacter::CallRespawnPlayer, RespawnInterval, false);
		}
	}
	else
	{
		ServerDie(InstigatorActor);
	}
}

void AMyProjectCharacter::ServerDie_Implementation(AActor* InstigatorActor)
{
	Die(InstigatorActor);
}

void AMyProjectCharacter::CallRespawnPlayer()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	UWorld* World = GetWorld();
	if (PC && World)
	{
		AMyProjectGameMode* GM = Cast<AMyProjectGameMode>(World->GetAuthGameMode());
		if (GM)
		{
			GM->RespawnPlayer(PC, SpawnTransform);
		}
	}
}

void AMyProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyProjectCharacter, GunToBePickedUp);
	DOREPLIFETIME(AMyProjectCharacter, KilledBy);
}