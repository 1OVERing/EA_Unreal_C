// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "IMC_Combat.generated.h"

class UInputAction;

UCLASS(BlueprintType, config = Input)
class EA_UNREAL_C_API UIMC_Combat : public UInputMappingContext
{
	GENERATED_BODY()
public:
	UIMC_Combat();

private:
	UInputAction* EquipAction;
	UInputAction* LMouseAction;
	UInputAction* RMouseAction;
	UInputAction* CatchAction;
	UInputAction* GuardAction;
public:
	FORCEINLINE UInputAction* GetEquipAction() const { return EquipAction; }
	FORCEINLINE UInputAction* GetLMouseAction() const { return LMouseAction; }
	FORCEINLINE UInputAction* GetRMouseAction() const { return RMouseAction; }
	FORCEINLINE UInputAction* GetCatchAction() const { return CatchAction; }
	FORCEINLINE UInputAction* GetGuardAction() const { return GuardAction; }
};
