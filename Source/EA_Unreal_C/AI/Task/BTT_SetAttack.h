// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetAttack.generated.h"

UCLASS()
class EA_UNREAL_C_API UBTT_SetAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SetAttack(FObjectInitializer const& ObjectInitializer);
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
