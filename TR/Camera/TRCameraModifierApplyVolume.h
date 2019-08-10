// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/HoatCameraModifier.h"
#include "Camera/TRCameraInfo.h"
#include "TRCameraModifierApplyVolume.generated.h"

class AHoatCameraModificationVolume;

UCLASS(BlueprintType, Blueprintable)
class TR_API UTRCameraModifierApplyVolume : public UHoatCameraModifier
{
	GENERATED_BODY()
	
public:
	UTRCameraModifierApplyVolume(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
};
