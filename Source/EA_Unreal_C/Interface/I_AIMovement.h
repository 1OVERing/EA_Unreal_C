// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_AIMovement.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_AIMovement : public UInterface
{
	GENERATED_BODY()
};

class EA_UNREAL_C_API II_AIMovement
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
		bool CustomMoveStart(FVector MoveToLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
		bool CustomMoveTo(FVector MoveToLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
		bool CustomMoveEnd(FVector MoveToLocation);
};
