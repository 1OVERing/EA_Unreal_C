// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "IMC_Movement.generated.h"

class UInputAction;

UCLASS()
class EA_UNREAL_C_API UIMC_Movement : public UInputMappingContext
{
	GENERATED_BODY()
public:
	UIMC_Movement();

private:
	UInputAction* MovementAction;
	UInputAction* LookAction;
	UInputAction* SprintAction;
	UInputAction* JumpAction;
public:
	FORCEINLINE UInputAction* GetMovementAction() const { return MovementAction; }
	FORCEINLINE UInputAction* GetLookAction() const { return LookAction; }
	FORCEINLINE UInputAction* GetSprintAction() const { return SprintAction; }
	FORCEINLINE UInputAction* GetJumpAction() const { return JumpAction; }
};
