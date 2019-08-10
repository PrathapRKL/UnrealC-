// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/HoatCameraModifier.h"
#include "TRCameraModifierFocusTargetActor.generated.h"

/**
 * Makes the camera look in the direction of the current target actor, if one exists.
 */

UCLASS(BlueprintType, Blueprintable)
class TR_API UTRCameraModifierFocusTargetActor : public UHoatCameraModifier
{
	GENERATED_BODY()

public:
	UTRCameraModifierFocusTargetActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation,
		FRotator& OutDeltaRot) override;

private:
	/*
	* Rate at which the camera should move while not snapping to a target, in degrees per second. 
	*/
	UPROPERTY(Category = Custom, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float RotationSpeed;

	/*
	* Rate at which the camera should snap to the target, in degrees per second. 
	*/
	UPROPERTY(Category = Custom, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float SnapSpeed;

	/*
	* Target actor that was selected at the time of the previous camera update. 
	*/
	AActor* LastTarget;

	/*
	* Desired rotation for focusing the target actor. 
	*/
	FRotator DesiredRotation;
};
