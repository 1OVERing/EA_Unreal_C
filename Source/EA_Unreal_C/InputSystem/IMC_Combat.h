// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "IMC_Combat.generated.h"

class UInputAction;

UCLASS()
class EA_UNREAL_C_API UIMC_Combat : public UInputMappingContext
{
	GENERATED_BODY()
public:
	UIMC_Combat();

private:
	UInputAction* EquipAction;
public:
	FORCEINLINE UInputAction* GetEquipAction() const { return EquipAction; }
};
