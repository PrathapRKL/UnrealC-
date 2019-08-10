// Fill out your copyright notice in the Description page of Project Settings.

#include "TRCameraModifierInCombat.h"
#include "Camera/CameraTypes.h"
#include "Camera/TRCombatActorInterface.h"

UTRCameraModifierInCombat::UTRCameraModifierInCombat(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UTRCameraModifierInCombat::ModifyCamera(float DeltaTime, FMinimalViewInfo & InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	// Get player character.
	ITRCombatActorInterface* CombatActor = Cast<ITRCombatActorInterface>(GetViewTarget());

	if (!CombatActor)
	{
		return false;
	}

	// Apply modifier.
	AppyCameraTransition(CombatActor->IsInCombat() ? Modifiers : FHoatCameraInfo(), TransitionTime, InOutPOV, DeltaTime);
	return false;
}
