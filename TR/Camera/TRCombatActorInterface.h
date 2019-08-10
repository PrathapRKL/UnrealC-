// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/ObjectMacros.h"
#include "TRCombatActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTRCombatActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class TR_API ITRCombatActorInterface
{
	GENERATED_IINTERFACE_BODY()

		virtual bool IsInCombat() const = 0;
};
