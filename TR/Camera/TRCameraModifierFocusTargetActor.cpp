// Fill out your copyright notice in the Description page of Project Settings.

#include "TRCameraModifierFocusTargetActor.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/TRTargetInterface.h"

UTRCameraModifierFocusTargetActor::UTRCameraModifierFocusTargetActor(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RotationSpeed = 300.0f;
	SnapSpeed = 0.3f;
}

bool UTRCameraModifierFocusTargetActor::ProcessViewRotation(AActor * ViewTarget, float DeltaTime, FRotator & OutViewRotation, FRotator & OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	// Check if we're selecting a target.
	if (!IsValid(ViewTarget))
	{
		return false;
	}

	ITRTargetInterface* TargetingActor = Cast<ITRTargetInterface>(ViewTarget);

	if (!TargetingActor)
	{
		return false;
	}

	if (!TargetingActor->IsSelectingTarget())
	{
		return false;
	}

	// Check if there's a selected target.
	AActor* CurrentTarget = TargetingActor->GetCurrentTarget();

	if (IsValid(CurrentTarget))
	{
		if (CurrentTarget != LastTarget)
		{
			// We have selected a new target. Compute desired rotation.
			FVector CameraLocation = CameraOwner->GetCameraLocation();
			FVector TargetLocation = CurrentTarget->GetActorLocation();

			// Get rotation delta from current camera rotation to look-at rotation for the target.
			// As the camera position might change depending on its rotation (e.g. for spring arms),
			// we just compute this once per target and store the result.
			DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);
			LastTarget = CurrentTarget;
		}

		FRotator CurrentRotation = OutViewRotation;

		if (SnapSpeed <= 0)
		{
			// Snap immediately.
			OutViewRotation = DesiredRotation;
		}
		else
		{
			FRotator TowardsDesired = DesiredRotation - CurrentRotation;
			FRotator TowardsDesiredNormalized = TowardsDesired.GetNormalized();

			if (TowardsDesiredNormalized.IsNearlyZero())
			{
				// Prevent overshooting.
				OutViewRotation = DesiredRotation;
			}
			else
			{
				// Apply rotation speed.
				FRotator DeltaRot = TowardsDesiredNormalized * SnapSpeed * DeltaTime;
				OutDeltaRot += DeltaRot;
			}
		}
	}
	else
	{
		LastTarget = nullptr;

		// No target selected. Smoothly apply player input.
		FVector2D TargetSelectionInput = TargetingActor->GetCurrentTargetSelectionInput();

		FRotator DeltaRot;
		DeltaRot.Yaw = TargetSelectionInput.X * RotationSpeed * DeltaTime;
		DeltaRot.Pitch = -TargetSelectionInput.Y * RotationSpeed * DeltaTime;
		DeltaRot.Roll = 0.0f;

		OutDeltaRot += DeltaRot;
	}

	// Prevent further camera modifiers from being applied.
	return true;
}
