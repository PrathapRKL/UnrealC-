// Fill out your copyright notice in the Description page of Project Settings.

#include "HoatCameraModifier.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Camera/TRSpringArmComponent.h"

UHoatCameraModifier::UHoatCameraModifier(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer),
	CurrentModifiers(FHoatCameraInfo()),
	TargetModifiers(FHoatCameraInfo())
{
}

bool UHoatCameraModifier::ProcessViewRotation(AActor * ViewTarget, float DeltaTime, FRotator & OutViewRotation, FRotator & OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	if (!IsValid(ViewTarget))
	{
		return false;
	}

	APawn* PlayerPawn = Cast<APawn>(ViewTarget);

	if (!IsValid(PlayerPawn))
	{
		return false;
	}

	APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->Controller);

	if (!IsValid(PlayerController))
	{
		return false;
	}

	if (!PlayerController->RotationInput.IsNearlyZero(THRESH_QUAT_NORMALIZED))
	{
		// Reset cooldown.
		CooldownRemaining = CooldownAfterPlayerInput;
		return false;
	}

	if (CooldownRemaining > 0)
	{
		// Tick cooldown.
		CooldownRemaining -= DeltaTime;
		return false;
	}
	return false;
}

FHoatCameraInfo UHoatCameraModifier::GetCurrentModifiers() const
{
	return CurrentModifiers;
}

FHoatCameraInfo UHoatCameraModifier::GetTargetModifiers() const
{
	return TargetModifiers;
}

void UHoatCameraModifier::AppyCameraTransition(const FHoatCameraInfo CameraInfo, const float TransitionTime, FMinimalViewInfo & InOutPOV, const float DeltaTime)
{
	if (!TargetModifiers.Equals(CameraInfo))
	{
		CurrentModifiers = TargetModifiers;
		TargetModifiers = CameraInfo;

		if (RemainingTransitionTime > 0.0f)
		{
			// New transition started before old was finished. Play transition backwards.
			RemainingTransitionTime = TotalTransitionTime - RemainingTransitionTime;
		}
	}

	if (CurrentModifiers.Equals(TargetModifiers))
	{
		// No changes.
		ApplyCameraInfo(CurrentModifiers, 1.0f, InOutPOV);
		return;
	}

	// Apply modifiers considering the following conditions.
	// 1. CurrentModifiers is zero, TargetModifiers is not zero -> Apply new modifiers.
	// 2. CurrentModifiers is not zero, TargetModifiers is zero -> Stop applying modifiers.
	// 3. CurrentModifiers is not zero, TargetModifiers is not zero -> Need transition from current modifiers to new modifiers.

	// Apply modifiers.
	float ApplicationFactor = 0.0f;
	FHoatCameraInfo ModifiersToApply;

	if (!TargetModifiers.IsZero())
	{
		// Cond 1: Apply new modifiers.
		ModifiersToApply = TargetModifiers;

		if (RemainingTransitionTime <= 0.0f)
		{
			// Start transition timer.
			TotalTransitionTime = TransitionTime;
			RemainingTransitionTime = TotalTransitionTime;
		}
		else
		{
			// Tick transition timer.
			RemainingTransitionTime -= DeltaTime;

			if (RemainingTransitionTime <= 0.0f)
			{
				// Transition finished.
				CurrentModifiers = TargetModifiers;
				ApplicationFactor = 1.0f;
			}
			else
			{
				ApplicationFactor = 1 - (RemainingTransitionTime / TotalTransitionTime);
			}
		}
	}
	else
	{
		// Case 2: Stop applying modifiers.
		ModifiersToApply = CurrentModifiers;

		if (RemainingTransitionTime <= 0.0f)
		{
			// Restart transition timer.
			RemainingTransitionTime = TotalTransitionTime;
			ApplicationFactor = 1.0f;
		}
		else
		{
			// Tick transition timer.
			RemainingTransitionTime -= DeltaTime;

			if (RemainingTransitionTime <= 0.0f)
			{
				// Transition finished.
				CurrentModifiers = TargetModifiers;
				ApplicationFactor = 0.0f;
			}
			else
			{
				// We want to gradually decrease the influence of the previous modifiers.
				ApplicationFactor = RemainingTransitionTime / TotalTransitionTime;
			}
		}
	}
	// Apply modifiers.
	ApplyCameraInfo(ModifiersToApply, ApplicationFactor, InOutPOV);
}

bool UHoatCameraModifier::PlayerHasRecentlyChangedCamera() const
{
	return CooldownRemaining > 0;
}

void UHoatCameraModifier::ApplyCameraInfo(const FHoatCameraInfo & CameraInfo, const float Factor, FMinimalViewInfo & InOutPOV) const
{
	AActor* ViewTarget = GetViewTarget();

	if (!IsValid(ViewTarget))
	{
		return;
	}

	TArray<USceneComponent*> Children;
	ViewTarget->GetRootComponent()->GetChildrenComponents(true, Children);

	// Apply FOV.
	float AppliedFOV = CameraInfo.FOV * Factor;
	InOutPOV.FOV = InOutPOV.FOV + AppliedFOV;

	// Apply spring arm length.
	for (int index = 0; index < Children.Num(); ++index)
	{
		UTRSpringArmComponent* SpringArmComponent = Cast<UTRSpringArmComponent>(Children[index]);

		if (IsValid(SpringArmComponent))
		{
			SpringArmComponent->TargetArmLengthModifier += CameraInfo.SpringArmLength * Factor;
			break;
		}
	}

	// Apply depth of field.
	if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFocalDistance))
	{
		InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
		InOutPOV.PostProcessSettings.DepthOfFieldFocalDistance += CameraInfo.DepthOfFieldFocalDistance * Factor;
	}

	if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFocalRegion))
	{
		InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
		InOutPOV.PostProcessSettings.DepthOfFieldFocalRegion += CameraInfo.DepthOfFieldFocalRegion * Factor;
	}

	if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldNearTransitionRegion))
	{
		InOutPOV.PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = true;
		InOutPOV.PostProcessSettings.DepthOfFieldNearTransitionRegion += CameraInfo.DepthOfFieldNearTransitionRegion * Factor;
	}

	if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFarTransitionRegion))
	{
		InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = true;
		InOutPOV.PostProcessSettings.DepthOfFieldFarTransitionRegion += CameraInfo.DepthOfFieldFarTransitionRegion * Factor;
	}

	if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldNearBlurSize))
	{
		InOutPOV.PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = true;
		InOutPOV.PostProcessSettings.DepthOfFieldNearBlurSize += CameraInfo.DepthOfFieldNearBlurSize * Factor;
	}

	if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFarBlurSize))
	{
		InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = true;
		InOutPOV.PostProcessSettings.DepthOfFieldFarBlurSize += CameraInfo.DepthOfFieldFarBlurSize * Factor;
	}
}
