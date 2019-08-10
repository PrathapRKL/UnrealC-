// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/ObjectMacros.h"
#include "TRTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTRTargetInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ITRTargetInterface
{
	GENERATED_IINTERFACE_BODY()

		virtual bool IsSelectingTarget() const = 0;
	virtual AActor* GetCurrentTarget() const = 0;
	virtual FVector2D GetCurrentTargetSelectionInput() const = 0;
};
