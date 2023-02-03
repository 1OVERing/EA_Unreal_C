#include "BTT_FindRandomLocation.h"

#include"BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIController.h"
#include "NavigationSystem.h"


UBTT_FindRandomLocation::UBTT_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("BTT_FindRandomLocation");
}

EBTNodeResult::Type UBTT_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();

	if (!Controller || !Controller->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	FVector Origin = Controller->GetBlackboardComponent()->GetValueAsVector(OriginLocation.SelectedKeyName);
	FNavLocation NavLocation;
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Controller->GetWorld());
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, SearchRadius, NavLocation))
	{
		Controller->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, NavLocation);
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}