#include "BTT_AttackStartAndWait.h"
#include "kismet/KismetSystemLibrary.h"
#include "kismet/KismetStringLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Interface/I_CombatInteraction.h"

UBTT_AttackStartAndWait::UBTT_AttackStartAndWait(FObjectInitializer const& ObjectInitializer)
{
    this->NodeName = TEXT("BTT_AIAttackAndWait");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_AttackStartAndWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();
	if (!Owner) return EBTNodeResult::Failed;


	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_CombatInteraction::StaticClass()))
	{
		II_CombatInteraction* _interface = Cast<II_CombatInteraction>(Owner);
		float AttackTime = _interface->Execute_PlayAttack(Owner);
		if (AttackTime == 0.f)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return EBTNodeResult::Failed;
		}
		TotalTimer = WaitTime + AttackTime;
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}
void UBTT_AttackStartAndWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	GEngine->AddOnScreenDebugMessage(9911, 10.f, FColor::Green,UKismetStringLibrary::Conv_FloatToString(TotalTimer));

	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn())
	{
		return FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();
	if (!::IsValid(Owner)) return FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_CombatInteraction::StaticClass()))
	{
		II_CombatInteraction* _interface = Cast<II_CombatInteraction>(Owner);
		bool AttackEnd = _interface->Execute_AttackEndCheck(Owner);

		if (AttackEnd)
		{
			_interface->Execute_SetNextAttack(Owner);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}

		TotalTimer -= DeltaSeconds;

		if (TotalTimer <= 0.f)
		{
			_interface->Execute_SetNextAttack(Owner);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else return FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}
EBTNodeResult::Type UBTT_AttackStartAndWait::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

    return EBTNodeResult::Succeeded;
}
