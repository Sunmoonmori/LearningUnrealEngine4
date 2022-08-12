// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectile.h"

// Sets default values
AMyProjectile::AMyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("MyProjectile"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &AMyProjectile::OnHit);
		CollisionComponent->InitSphereRadius(15.0f);
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 1000.0f;
		ProjectileMovementComponent->MaxSpeed = 1000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if(!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Geometry/Meshes/Sphere.Sphere'"));
		if(Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/Geometry/Meshes/SphereMaterial.SphereMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AMyProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    Destroy();
}