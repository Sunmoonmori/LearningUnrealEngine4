// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGun.h"
#include "../Character/MyProjectCharacter.h"

// Sets default values
AMyGun::AMyGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
		MeshComponent->SetCollisionProfileName(TEXT("WorldGun")); // or BodyInstance?
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetupAttachment(RootComponent);
	}

	if (!OverlapComponent)
	{
		OverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		OverlapComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // or BodyInstance?
		OverlapComponent->SetupAttachment(MeshComponent);
	}

	bIsFiring = false;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AMyGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyGun::OnGunPickedUp()
{
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	OverlapComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	OverlapComponent->AttachToComponent(MeshComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
}

void AMyGun::OnGunDropped()
{
	OverlapComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MeshComponent->SetCollisionProfileName(TEXT("WorldGun"));
	MeshComponent->SetSimulatePhysics(true);
}

void AMyGun::StartFire(AActor* FireInstigator)
{
	if (!ProjectileClass)
	{
		return;
	}

	if (bIsFiring)
	{
		return;
	}

	bIsFiring = true;

	AMyProjectCharacter* FireInstigatorCharacter = Cast<AMyProjectCharacter>(FireInstigator);

	if (!FireInstigatorCharacter)
	{
		return;
	}

	UMyCharacterAttributeComponent* InstigatorAttributeComp = FireInstigatorCharacter->AttributeComp;

	if (InstigatorAttributeComp->GetMagicPoint() < 0.f)
	{
		return;
	}

	FVector MuzzleLocation = FireInstigatorCharacter->GetMesh()->GetSocketLocation(TEXT("MyMuzzleSocket"));
	FRotator MuzzleRotation = FireInstigatorCharacter->bUseControllerRotationYaw ?
		(FireInstigatorCharacter->AimingTargetLocation - MuzzleLocation).Rotation() : GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = FireInstigatorCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMyProjectile* Projectile = GetWorld()->SpawnActor<AMyProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
	if (Projectile)
	{
		FVector LaunchDirection = MuzzleRotation.Vector();
		Projectile->FireInDirection(LaunchDirection);
	}

	// consume MP
	InstigatorAttributeComp->ApplyMagicPointChange(this, -InstigatorAttributeComp->GetMagicPointConsumed());
}

void AMyGun::StopFire()
{
	bIsFiring = false;
}
