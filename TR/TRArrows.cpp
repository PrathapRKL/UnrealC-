
// Fill out your copyright notice in the Description page of Project Settings.

#include "TRArrows.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerChar.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"

// Sets default values
ATRArrows::ATRArrows()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit Box"));
	RootComponent = HitBox;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Mesh"));
	ArrowMesh->SetupAttachment(HitBox);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Component"));
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 4400.0f;

	CollSP = CreateDefaultSubobject<USphereComponent>(TEXT("Hit Collision Sphere"));
	CollSP->SetupAttachment(ArrowMesh);
}

// Called when the game starts or when spawned
void ATRArrows::BeginPlay()
{
	Super::BeginPlay();

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ATRArrows::OnOverlapBegin);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &ATRArrows::OnOverlapEnd);
	CollSP->OnComponentHit.AddDynamic(this, &ATRArrows::OnHit);
}

// Called every frame
void ATRArrows::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATRArrows::OnOverlapBegin(UPrimitiveComponent * OwnerComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool Teleport, const FHitResult & SweepResult)
{
}

void ATRArrows::OnOverlapEnd(UPrimitiveComponent * OwnerComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void ATRArrows::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	UGameplayStatics::ApplyDamage(OtherActor, Damage, this->GetInstigatorController(), this, DamageType_RegArrow);
}

