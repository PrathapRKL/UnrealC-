// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/ObjectMacros.h"
#include "TRCameraInterface.generated.h"

class AHoatCameraModificationVolume;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTRCameraInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ITRCameraInterface
{
	GENERATED_IINTERFACE_BODY()

		virtual AHoatCameraModificationVolume* GetCurrentCameraModificationVolume() const = 0;
	virtual void SetCurrentCameraModificationVolume(AHoatCameraModificationVolume* CurrentCameraModificationVolume) = 0;
};
