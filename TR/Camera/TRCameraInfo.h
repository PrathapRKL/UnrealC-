// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TRCameraInfo.generated.h"

/*
* Common camera properties that can be added or overridden for a camera. 
*/
USTRUCT(BlueprintType)
struct FHoatCameraInfo
{
	GENERATED_USTRUCT_BODY()

		/*
		* The field of view (in degrees) in perspective mode.
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float FOV;

	/*
	* Distance between the camera and the view target, in cm. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float SpringArmLength;

	/*
	* Distance in which the Depth of Field effect should be sharp, in uu(cm) 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
		float DepthOfFieldFocalDistance;

	/*
	* Artificial region where all content is in focus. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
		float DepthOfFieldFocalRegion;

	/*
	* To define the width of the transition region next to the focal region on the near side (cm) 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
		float DepthOfFieldNearTransitionRegion;

	/*
	* To define the width of the transition region next to the focal region on the near side (cm) 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
		float DepthOfFieldFarTransitionRegion;

	/*
	* Gaussian only: Maximum size of the Depth of Field blur (in percent of the view width).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
		float DepthOfFieldNearBlurSize;

	/*
	* Gaussian only: Maximum size of the Depth of Field blur (in percent of the view width). 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
		float DepthOfFieldFarBlurSize;

	/*
	* Compares both camera property sets for equality. 
	*/
	bool Equals(const FHoatCameraInfo& Other) const;

	/*
	* Whether this property set modifies the camera at all. 
	*/
	bool IsZero() const;
};
