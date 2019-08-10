// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/HoatCameraModifier.h"
#include "TRCameraModifierInCombat.generated.h"

/**
 * Applies modifications while the player is in combat.
 */

UCLASS(BlueprintType, Blueprintable)
class TR_API UTRCameraModifierInCombat : public UHoatCameraModifier
{
	GENERATED_BODY()

public:
	UTRCameraModifierInCombat(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;

	/*
	* Time until the modifications are fully applied, in seconds. 
	*/
	UPROPERTY(Category = CameraModifiers, BlueprintReadWrite, EditAnywhere)
		float TransitionTime;

	/*
	* Value to add to the current properties of the camera of any player in combat. 
	*/
	UPROPERTY(Category = CameraModifiers, BlueprintReadWrite, EditAnywhere)
		FHoatCameraInfo Modifiers;
};
