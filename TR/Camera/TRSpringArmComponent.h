// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "TRSpringArmComponent.generated.h"

/**
 * Extends the default Unreal Engine spring arm camera behavior to match the specific needs of the dynamic camera.
 */
UCLASS(Blueprintable, ClassGroup = (Custom))
class TR_API UTRSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	UTRSpringArmComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* Natural length of the spring arm when there are no collisions. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float BaseTargetArmLength;

	/*
	* Value to add to the target arm length. 
	*/
	float TargetArmLengthModifier;

protected:
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag,
		float DeltaTime) override;
};
