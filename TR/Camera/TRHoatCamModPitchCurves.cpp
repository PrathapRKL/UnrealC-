// Fill out your copyright notice in the Description page of Project Settings.

#include "TRHoatCamModPitchCurves.h"
#include "Camera/CameraTypes.h"
#include "Camera/TRPlayerCameraManager.h"

UTRHoatCamModPitchCurves::UTRHoatCamModPitchCurves(
	const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
}

bool UTRHoatCamModPitchCurves::ModifyCamera(float DeltaTime, FMinimalViewInfo & InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	ATRPlayerCameraManager* CameraManager = Cast<ATRPlayerCameraManager>(CameraOwner);

	if (!IsValid(CameraManager))
	{
		return false;
	}

	// Get camera location and rotation.
	const FVector CameraLocation = InOutPOV.Location;
	const FRotator CameraRotation = InOutPOV.Rotation;

	// Apply curves.
	const float PitchToDistance = IsValid(PitchToDistanceCurve) ? PitchToDistanceCurve->GetFloatValue(CameraRotation.Pitch) : 0.0f;
	const float PitchToFov = IsValid(PitchToFOVCurve) ? PitchToFOVCurve->GetFloatValue(CameraRotation.Pitch) : 0.0f;

	// Drive camera away based on evaluated curve.
	const FVector DesiredLocation =
		CameraLocation - CameraRotation.RotateVector(FVector::ForwardVector) * PitchToDistance;

	FVector NewLocation;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetViewTarget());
	FHitResult Results;
	GetWorld()->SweepSingleByChannel(Results, GetViewTarget()->GetActorLocation(), DesiredLocation, FQuat::Identity,
		CameraManager->LineOfSightProbeChannel,
		FCollisionShape::MakeSphere(CameraManager->LineOfSightProbeSize), QueryParams);
	NewLocation = Results.bBlockingHit ? Results.Location : DesiredLocation;

	InOutPOV.Location = NewLocation;
	InOutPOV.FOV += PitchToFov;
	return false;
}
