// Fill out your copyright notice in the Description page of Project Settings.

#include "TR_IncendiaryArrows.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerChar.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Classes/GameFramework/DamageType.h"
#include "ConstructorHelpers.h"

// Sets default values
ATR_IncendiaryArrows::ATR_IncendiaryArrows()
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


	// Getting the blueprint version of the class and assigning it to a variable belonging to the PlayerChar class.
	ConstructorHelpers::FObjectFinder<UBlueprint> BP_Incen_Arrows(TEXT("Blueprint'/Game/BPs/BP_FireArrow.BP_FireArrow'"));
	if (BP_Incen_Arrows.Object) {
		BP_Class = (UClass*)BP_Incen_Arrows.Object->GeneratedClass;
	}
}

// Called when the game starts or when spawned
void ATR_IncendiaryArrows::BeginPlay()
{
	Super::BeginPlay();
	
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ATR_IncendiaryArrows::OnOverlapBegin);
	HitBox->OnComponentHit.AddDynamic(this, &ATR_IncendiaryArrows::OnHit);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &ATR_IncendiaryArrows::OnOverlapEnd);
}

// Called every frame
void ATR_IncendiaryArrows::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATR_IncendiaryArrows::OnOverlapBegin(UPrimitiveComponent * OwnerComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool Teleport, const FHitResult & SweepResult)
{
}

void ATR_IncendiaryArrows::OnOverlapEnd(UPrimitiveComponent * OwnerComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void ATR_IncendiaryArrows::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	TArray<AActor*> IgnoredActors;
	APlayerChar* PlayerCharacter = Cast<APlayerChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	IgnoredActors.Add(PlayerCharacter);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), 100.0f, DamageType_Fire, IgnoredActors);
}

