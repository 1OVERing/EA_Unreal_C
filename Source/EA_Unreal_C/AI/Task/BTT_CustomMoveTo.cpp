#include "BTT_CustomMoveTo.h"
#include "kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Interface/I_AIMovement.h"

UBTT_CustomMoveTo::UBTT_CustomMoveTo(FObjectInitializer const& ObjectInitializer)
{
	this->NodeName = TEXT("BTT_CustomMoveTo");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_CustomMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();
	FVector Location = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector(BlackboardKey.SelectedKeyName);
	AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TargetActor.SelectedKeyName));
	if (::IsValid(Target))
	{
		PrevActorLocation = Target->GetActorLocation();
	}

	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_AIMovement::StaticClass()))
	{
		II_AIMovement* _interface = Cast<II_AIMovement>(Owner);
		bool result = _interface->Execute_CustomMoveStart(Owner,Location);
		if (!result)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTT_CustomMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	
	AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TargetActor.SelectedKeyName));
	if (::IsValid(Target) && PrevActorLocation != Target->GetActorLocation())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();
	FVector Location = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector(BlackboardKey.SelectedKeyName);
	II_AIMovement* _interface = Cast<II_AIMovement>(Owner);

	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_AIMovement::StaticClass()))
	{
		bool Finish = _interface->Execute_CustomMoveTo(Owner,Location);
		if (Finish)
		{
			_interface->Execute_CustomMoveEnd(Owner,Location);

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		_interface->Execute_CustomMoveEnd(Owner,Location);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UBTT_CustomMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);
	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();
	FVector Location = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector(BlackboardKey.SelectedKeyName);
	II_AIMovement* _interface = Cast<II_AIMovement>(Owner);

	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_AIMovement::StaticClass()))
	{
		bool Finish = _interface->Execute_CustomMoveEnd(Owner,Location);
	}

	return EBTNodeResult::Succeeded;
}