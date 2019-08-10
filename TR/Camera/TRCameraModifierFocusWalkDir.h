// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/HoatCameraModifier.h"
#include "TRCameraModifierFocusWalkDir.generated.h"

/**
 * Makes the camera always look in walk direction.
 */
UCLASS(BlueprintType, Blueprintable)
class TR_API UTRCameraModifierFocusWalkDir : public UHoatCameraModifier
{
	GENERATED_BODY()

public:
	UTRCameraModifierFocusWalkDir(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

private:
	/*
	* Rate at which the camera should follow the turning character, in degrees per second. 
	*/
	UPROPERTY(Category = Custom, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float RotationSpeed;

	/*
	* Minimum time between two changes in direction of automatic camera rotation, in seconds. 
	*/
	UPROPERTY(Category = Custom, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float DirectionChangeCooldown;

	/*
	* Whether to always rotate the camera to focus walk direction, or only while the player is providing movement input. 
	*/
	UPROPERTY(Category = Custom, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		bool RotateOnlyWhileCharacterIsMoving;

public:

	float PreviousYawDeltaSign;
	float DirectionChangeCooldownRemaining;
};
