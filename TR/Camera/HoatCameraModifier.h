// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "Camera/TRCameraInfo.h"
#include "HoatCameraModifier.generated.h"

/**
 *  Applies HOAT camera modifications over time.
 */
UCLASS(BlueprintType, Blueprintable)
class TR_API UHoatCameraModifier : public UCameraModifier
{
	GENERATED_BODY()

public:
	UHoatCameraModifier(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	/** Gets the camera modifiers that are currently being applied. */
	FHoatCameraInfo GetCurrentModifiers() const;

	/** Gets the camera modifiers that we are currently transitioning to. */
	FHoatCameraInfo GetTargetModifiers() const;

	/** How long to wait after player has changed the camera before automatically changing it again, in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float CooldownAfterPlayerInput;

	void AppyCameraTransition(const FHoatCameraInfo CameraInfo, const float TransitionTime, struct FMinimalViewInfo& InOutPOV, const float DeltaTime);

	/** Whether the player has recently changed the camera. Check this in order to have their input always have priority over automatic adjustments. */
	bool PlayerHasRecentlyChangedCamera() const;

private:
	void ApplyCameraInfo(const FHoatCameraInfo& CameraInfo, const float Factor, struct FMinimalViewInfo& InOutPOV) const;

	FHoatCameraInfo CurrentModifiers;
	FHoatCameraInfo TargetModifiers;
	float TotalTransitionTime;
	float RemainingTransitionTime;

	/** Time before we automatically adjust the camera again, in seconds. */
	float CooldownRemaining;
};
