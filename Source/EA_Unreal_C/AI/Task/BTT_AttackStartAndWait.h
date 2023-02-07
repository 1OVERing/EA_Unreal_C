#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AttackStartAndWait.generated.h"

UCLASS()
class EA_UNREAL_C_API UBTT_AttackStartAndWait : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_AttackStartAndWait(FObjectInitializer const& ObjectInitializer);
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float WaitTime = 1.f;

	float TotalTimer = 0.f;
};
