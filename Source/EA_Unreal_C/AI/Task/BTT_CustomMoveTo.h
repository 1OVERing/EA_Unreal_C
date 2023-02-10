// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_CustomMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class EA_UNREAL_C_API UBTT_CustomMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_CustomMoveTo(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetActor;
	FVector PrevActorLocation;
};
