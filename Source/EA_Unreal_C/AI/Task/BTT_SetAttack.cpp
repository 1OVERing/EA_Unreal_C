// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetAttack.h"
#include "../../Interface/I_CombatInteraction.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"

UBTT_SetAttack::UBTT_SetAttack(FObjectInitializer const& ObjectInitializer)
{
	this->NodeName = "BTT_SetAttack";
}

EBTNodeResult::Type UBTT_SetAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		bool Result = _interface->Execute_SetNextAttack(Owner);
		if (Result)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
		else
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
