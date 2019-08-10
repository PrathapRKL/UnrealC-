// Fill out your copyright notice in the Description page of Project Settings.

#include "TRSpringArmComponent.h"

UTRSpringArmComponent::UTRSpringArmComponent(
	const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	// Enable camera controls.
	bUsePawnControlRotation = true;

	// Set reasonable default values for camera distance and perspective.
	BaseTargetArmLength = 100.0f;

	// Enable camera lag.
	bEnableCameraLag = true;
	CameraLagSpeed = 4.0f;
}

void UTRSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	TargetArmLength = BaseTargetArmLength - TargetArmLengthModifier;
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
	TargetArmLengthModifier = 0.0f;
}
