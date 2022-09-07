// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGun.h"
#include "MyProjectCharacter.h"

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

void AMyGun::Interaction(AActor* InteractionInstigator)
{
	AMyProjectCharacter* InstigatorCharacter = Cast<AMyProjectCharacter>(InteractionInstigator);
	if (InstigatorCharacter)
	{
		InstigatorCharacter->PickUpGun(this);
	}
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
