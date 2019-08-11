// Fill out your copyright notice in the Description page of Project Settings.

#include "TR_ExplosiveArrows.h"
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
ATR_ExplosiveArrows::ATR_ExplosiveArrows()
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
	CollSP->SetGenerateOverlapEvents(true);
	CollSP->SetNotifyRigidBodyCollision(true);


	// Getting the blueprint version of the class and assigning it to a variable belonging to the PlayerChar class.
	ConstructorHelpers::FObjectFinder<UBlueprint> BP_Exp_Arrows(TEXT("Blueprint'/Game/BPs/BP_ExpArrow.BP_ExpArrow'"));
	if (BP_Exp_Arrows.Object) {
		BP_Class = (UClass*)BP_Exp_Arrows.Object->GeneratedClass;
		/*APlayerChar* PlayerCharacter = Cast<APlayerChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		PlayerCharacter->Exp_Arrows = BP_Class;*/
	}
}

// Called when the game starts or when spawned
void ATR_ExplosiveArrows::BeginPlay()
{
	Super::BeginPlay();
	
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ATR_ExplosiveArrows::OnOverlapBegin);
	HitBox->OnComponentHit.AddDynamic(this, &ATR_ExplosiveArrows::OnHit);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &ATR_ExplosiveArrows::OnOverlapEnd);
}

// Called every frame
void ATR_ExplosiveArrows::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATR_ExplosiveArrows::OnOverlapBegin(UPrimitiveComponent * OwnerComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool Teleport, const FHitResult & SweepResult)
{
}

void ATR_ExplosiveArrows::OnOverlapEnd(UPrimitiveComponent * OwnerComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void ATR_ExplosiveArrows::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	TArray<AActor*> IgnoredActors;
	APlayerChar* PlayerCharacter = Cast<APlayerChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	IgnoredActors.Add(PlayerCharacter);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), 100.0f, DamageType_Exp, IgnoredActors);
}

