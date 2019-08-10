// Fill out your copyright notice in the Description page of Project Settings.

#include "TRCameraModifierKeepLOS.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Camera/TRPlayerCameraManager.h"

UTRCameraModifierKeepLOS::UTRCameraModifierKeepLOS(
	const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	MaximumAngleInRadians = PI / 2;
	StepSizeInRadians = 0.1f;

	RotationSpeed = 1.0f;

	LineOfSightProbeChannel = ECC_Camera;
}

bool UTRCameraModifierKeepLOS::ProcessViewRotation(AActor * ViewTarget, float DeltaTime, FRotator & OutViewRotation, FRotator & OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	if (PlayerHasRecentlyChangedCamera())
	{
		return false;
	}

	// Get the camera location and rotation.
	const FVector DesiredLocation = CameraOwner->GetCameraLocation();
	const FRotator DesiredRotation = CameraOwner->GetCameraRotation();

	// Check if target is visible for the camera. Shoot "whiskers" to the left and right.
	const FVector TargetLocation = GetViewTarget()->GetActorLocation();

	float DesiredAngleInRadians = 0.0f;
	float CheckAngleInRadians = 0.0f;

	// Check from the left. Start very close to the actual camera angle:
	// This is the most severe case, in which we might need to make a big swing away.
	CheckAngleInRadians =- StepSizeInRadians;

	while (CheckAngleInRadians >= -MaximumAngleInRadians)
	{
		FVector TestLocation = DesiredLocation;
		FRotator TestRotation = DesiredRotation;

		RotateAroundLocation(TargetLocation, TestLocation, TestRotation, CheckAngleInRadians);

		// Check if line of sight blocked.
		if (!IsInLineOfSight(TargetLocation, TestLocation))
		{
			// We found an obstacle for this angle.
			// The smaller the absolute value of CheckAngleInRadians,
			// the closer the obstacle, and the further we need to swing away.
			DesiredAngleInRadians += CheckAngleInRadians + MaximumAngleInRadians;
			break;
		}

		// Look further away.
		CheckAngleInRadians -= StepSizeInRadians;
	}

	// Check from right.
	CheckAngleInRadians = StepSizeInRadians;

	while (CheckAngleInRadians <= MaximumAngleInRadians)
	{
		FVector TestLocation = DesiredLocation;
		FRotator TestRotation = DesiredRotation;

		RotateAroundLocation(TargetLocation, TestLocation, TestRotation, CheckAngleInRadians);

		// Check if line of sight blocked.
		if (!IsInLineOfSight(TargetLocation, TestLocation))
		{
			DesiredAngleInRadians += CheckAngleInRadians - MaximumAngleInRadians;
			break;
		}

		// Look further away.
		CheckAngleInRadians += StepSizeInRadians;
	}

	// Apply rotation.
	const float AppliedAngleInRadians =
		FMath::Clamp(DesiredAngleInRadians, -RotationSpeed * DeltaTime, +RotationSpeed * DeltaTime);

	OutDeltaRot.Yaw += FMath::RadiansToDegrees(AppliedAngleInRadians);

	return false;
}

bool UTRCameraModifierKeepLOS::ModifyCamera(float DeltaTime, FMinimalViewInfo & InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	return false;
}

bool UTRCameraModifierKeepLOS::IsInLineOfSight(const FVector & From, const FVector & To) const
{
	UWorld* world = GetWorld();
	if (!IsValid(world))
	{
		return false;
	}

	ATRPlayerCameraManager* CameraManager = Cast<ATRPlayerCameraManager>(CameraOwner);

	if (!IsValid(CameraManager))
	{
		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetViewTarget());
	FHitResult Results;
	world->SweepSingleByChannel(Results, From, To, FQuat::Identity, LineOfSightProbeChannel,
		FCollisionShape::MakeSphere(CameraManager->LineOfSightProbeSize), QueryParams);

	return !Results.bBlockingHit;
}

void UTRCameraModifierKeepLOS::RotateAroundLocation(const FVector & TargetLocation, FVector & InOutViewLocation, FRotator & InOutViewRotation, const float AngleInRadians)
{
	// Add angle.
	FRotator NewRotation = InOutViewRotation;
	NewRotation.Yaw += FMath::RadiansToDegrees(AngleInRadians);
	InOutViewRotation = NewRotation;

	// Rotate test location around pawn.
	FVector NewLocation = InOutViewLocation;

	const float AngleSin = FMath::Sin(AngleInRadians);
	const float AngleCos = FMath::Cos(AngleInRadians);

	// Translate point to origin.
	NewLocation.X -= TargetLocation.X;
	NewLocation.Y -= TargetLocation.Y;

	// Rotate point.
	const float RotatedX = NewLocation.X * AngleCos - NewLocation.Y * AngleSin;
	const float RotatedY = NewLocation.X * AngleSin + NewLocation.Y * AngleCos;

	// Translate point back.
	NewLocation.X = RotatedX + TargetLocation.X;
	NewLocation.Y = RotatedY + TargetLocation.Y;

	InOutViewLocation = NewLocation;
}
