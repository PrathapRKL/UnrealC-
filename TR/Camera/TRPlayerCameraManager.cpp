// Fill out your copyright notice in the Description page of Project Settings.

#include "TRPlayerCameraManager.h"

ATRPlayerCameraManager::ATRPlayerCameraManager()
{
	LineOfSightProbeSize = 12.0f;
	LineOfSightProbeChannel = ECC_Camera;
}

void ATRPlayerCameraManager::UpdateViewTarget(FTViewTarget & OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
}
