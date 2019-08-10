// Fill out your copyright notice in the Description page of Project Settings.

#include "TRCameraModifierFocusWalkDir.h"
#include "GameFramework/Actor.h"
#include "Camera/TRPlayerCharacterInterface.h"


UTRCameraModifierFocusWalkDir::UTRCameraModifierFocusWalkDir(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	RotationSpeed = 10.0f;
}
bool UTRCameraModifierFocusWalkDir::ModifyCamera(float DeltaTime, FMinimalViewInfo & InOutPOV)
{
	return Super::ModifyCamera(DeltaTime, InOutPOV);
}

bool UTRCameraModifierFocusWalkDir::ProcessViewRotation(AActor * ViewTarget, float DeltaTime, FRotator & OutViewRotation, FRotator & OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	// Tick cooldown.
	if (DirectionChangeCooldownRemaining > 0)
	{
		DirectionChangeCooldownRemaining -= DeltaTime;
	}

	ITRPlayerCharacterInterface* PlayerCharacter = Cast<ITRPlayerCharacterInterface>(ViewTarget);

	if (!PlayerCharacter)
	{
		return false;
	}

	// Check if we should apply automatic rotation.
	if (RotateOnlyWhileCharacterIsMoving && !PlayerCharacter->GotMovementInput())
	{
		return false;
	}

	if (PlayerHasRecentlyChangedCamera())
	{
		return false;
	}

	// Get current actor and view rotations.
	const float ActorYaw = ViewTarget->GetActorRotation().Yaw;
	const float ViewYaw = OutViewRotation.Yaw;

	// Always take the "short route" while rotating.
	float YawDelta = ActorYaw - ViewYaw;

	while (YawDelta < -180.0f)
	{
		YawDelta += 360.0f;
	}

	while (YawDelta > 180.0f)
	{
		YawDelta -= 360.0f;
	}

	// Check direction of rotation.
	float YawDeltaSign = FMath::Sign(YawDelta);

	if (PreviousYawDeltaSign != YawDeltaSign)
	{
		if (DirectionChangeCooldownRemaining > 0)
		{
			return false;
		}
		else
		{
			PreviousYawDeltaSign = YawDeltaSign;
			DirectionChangeCooldownRemaining = DirectionChangeCooldown;
		}
	}

	// Apply rotation speed.
	float AppliedYawDelta = YawDeltaSign * RotationSpeed * DeltaTime;

	// Prevent flipping back and forth for very small deltas.
	if (FMath::Abs(YawDelta) < FMath::Abs(AppliedYawDelta))
	{
		PreviousYawDeltaSign = 0.0f;
		DirectionChangeCooldownRemaining = DirectionChangeCooldown;
		AppliedYawDelta = YawDelta;
	}

	OutDeltaRot.Yaw += AppliedYawDelta;

	return false;
}
