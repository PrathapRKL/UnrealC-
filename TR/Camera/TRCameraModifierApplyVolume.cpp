// Fill out your copyright notice in the Description page of Project Settings.

#include "TRCameraModifierApplyVolume.h"
#include "Camera/CameraTypes.h"
#include "Camera/TRCameraInterface.h"
#include "Camera/HoatCameraModificationVolume.h"

UTRCameraModifierApplyVolume::UTRCameraModifierApplyVolume(
	const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
}

bool UTRCameraModifierApplyVolume::ModifyCamera(float DeltaTime, FMinimalViewInfo & InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	// Get player character.
	ITRCameraInterface* CameraActor = Cast<ITRCameraInterface>(GetViewTarget());

	if (!CameraActor)
	{
		return false;
	}

	// Apply volume.
	const FHoatCameraInfo PlayerCameraModificationVolumeInfo = CameraActor->GetCurrentCameraModificationVolume()
		? CameraActor->GetCurrentCameraModificationVolume()->Modifiers
		: FHoatCameraInfo();

	const float TransitionTime = CameraActor->GetCurrentCameraModificationVolume()
		? CameraActor->GetCurrentCameraModificationVolume()->TransitionTime
		: 0.0f;

	AppyCameraTransition(PlayerCameraModificationVolumeInfo, TransitionTime, InOutPOV, DeltaTime);

	return false;
}
