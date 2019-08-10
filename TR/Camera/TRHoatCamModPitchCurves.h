// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/HoatCameraModifier.h"
#include "TRHoatCamModPitchCurves.generated.h"

UCLASS(BlueprintType, Blueprintable)
class TR_API UTRHoatCamModPitchCurves : public UHoatCameraModifier
{
	GENERATED_BODY()

public:
	UTRHoatCamModPitchCurves(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;

private:
	// Curve to convert changes in pitch to changes in camera distance.
	UPROPERTY(Category = Custom, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCurveFloat* PitchToDistanceCurve;

	// Curve to convert changes in pitch to changes in field of view.
	UPROPERTY(Category = Custom, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCurveFloat* PitchToFOVCurve;
};
