#pragma once
#include "EngineMinimal.h"

namespace CustomMath
{
	UFUNCTION(BlueprintCallable,Category = "CustomMath")
	FVector2D FindVectorToDirection(const AActor* OwnerActor, const FVector& TargetLocation);
	UFUNCTION(BlueprintCallable, Category = "CustomMath")
	float GetTargetAngle(const AActor* OwnerActor, const FVector& TargetLocation);
}