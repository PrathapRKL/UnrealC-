// Fill out your copyright notice in the Description page of Project Settings.

#include "TR_Interactable_Base.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerChar.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Components/WidgetComponent.h"

// Sets default values
ATR_Interactable_Base::ATR_Interactable_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Component"));
	RootComponent = CollisionComponent;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable Mesh"));
	InteractableMesh->SetupAttachment(RootComponent);

	ItemName = FString("Item Name");
	InteractableText = FString("Press E to interact.");

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractWidget->SetHiddenInGame(true);
	InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

// Called when the game starts or when spawned
void ATR_Interactable_Base::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATR_Interactable_Base::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ATR_Interactable_Base::OnOverlapEnd);
}

// Called every frame
void ATR_Interactable_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATR_Interactable_Base::Use()
{
}

void ATR_Interactable_Base::Interact()
{
	ItemCount += 1;
	Destroy();
}

void ATR_Interactable_Base::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APlayerChar* PlayerCharacter = Cast<APlayerChar>(OtherActor);
	if (OtherActor == PlayerCharacter)
	{
		InteractWidget->SetHiddenInGame(false);
		PlayerCharacter->bCanPickUp = true;
	}
}

void ATR_Interactable_Base::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	APlayerChar* PlayerCharacter = Cast<APlayerChar>(OtherActor);
	if (OtherActor == PlayerCharacter)
	{
		InteractWidget->SetHiddenInGame(true);
		PlayerCharacter->bCanPickUp = false;
	}
}

