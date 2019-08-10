// Fill out your copyright notice in the Description page of Project Settings.

#include "HoatCameraModificationVolume.h"
#include "Camera/TRCameraInterface.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "PlayerChar.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/TRSpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

AHoatCameraModificationVolume::AHoatCameraModificationVolume(
	const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	TransitionTime = 1.0f;
}

void AHoatCameraModificationVolume::NotifyActorBeginOverlap(AActor * Other)
{
	Super::NotifyActorBeginOverlap(Other);

	APlayerChar* PlayerCharacter = Cast<APlayerChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	ITRCameraInterface* CameraActor = Cast<ITRCameraInterface>(Other);

	UTRSpringArmComponent* SpringArm = Cast<UTRSpringArmComponent>(PlayerCharacter->CameraBoom);

	if (!CameraActor)
	{
		return;
	}

	// Set camera modification volume.
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap success"));
	CameraActor->SetCurrentCameraModificationVolume(this);
	SpringArm->TargetOffset = FVector(0.0f, 0.0f, 100.0f);
	PlayerCharacter->bCanAim = false;
	TArray<AActor*> Actors;
	PlayerCharacter->GetOverlappingActors(Actors);
	for (AActor* ResultActor : Actors)
	{
		if (ResultActor == this)
		{
			PlayerCharacter->ModifySpringArm();
		}
	}
}

void AHoatCameraModificationVolume::NotifyActorEndOverlap(AActor * Other)
{
	Super::NotifyActorEndOverlap(Other);

	APlayerChar* PlayerCharacter = Cast<APlayerChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	ITRCameraInterface*  CameraActor = Cast<ITRCameraInterface>(Other);

	UTRSpringArmComponent* SpringArm = Cast<UTRSpringArmComponent>(PlayerCharacter->CameraBoom);

	if (!CameraActor)
	{
		return;
	}

	// Reset camera modification volume.
	if (CameraActor->GetCurrentCameraModificationVolume() == this)
	{
		CameraActor->SetCurrentCameraModificationVolume(nullptr);
		SpringArm->TargetOffset = FVector(0.0f, 0.0f, 0.0f);
		SpringArm->CameraLagSpeed = 3.0f;
		PlayerCharacter->CamTL.Reverse();
		PlayerCharacter->bCanAim = true;
		PlayerCharacter->bCanSprint = true;
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 150.0f;
		APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PlayerController->PlayerCameraManager->ViewYawMax = 359.998993f;
		PlayerController->PlayerCameraManager->ViewYawMin = 0.0f;
	}
}
