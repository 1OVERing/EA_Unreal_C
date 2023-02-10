#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveToTarget.generated.h"

UCLASS()
class EA_UNREAL_C_API UBTT_MoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_MoveToTarget(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector AllowableMinRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector AllowableMaxRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector MoveToTarget;
	
	FVector SaveLocation = FVector::ZeroVector;
};
