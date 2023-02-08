#include "BTT_MoveToTarget.h"
#include "kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Interface/I_AIMovement.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

UBTT_MoveToTarget::UBTT_MoveToTarget(FObjectInitializer const& ObjectInitializer)
{
	this->NodeName = TEXT("BTT_MoveToTarget");
	bNotifyTick = true;
}
EBTNodeResult::Type UBTT_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	ACharacter* Owner = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	ACharacter* Target = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(MoveToTarget.SelectedKeyName));

	if(!Owner || !Target) return EBTNodeResult::Failed;

	float Range = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsFloat(AllowableRange.SelectedKeyName);

	float Distance = FVector2D::Distance(FVector2D(Owner->GetActorLocation()), FVector2D(Target->GetActorLocation()));
	float Radius = Owner->GetCapsuleComponent()->GetScaledCapsuleRadius() + Target->GetCapsuleComponent()->GetScaledCapsuleRadius();
	Distance -= Radius;
	if (Range >= Distance)
	{
		return EBTNodeResult::Succeeded;
	}



	FVector Location = Target->GetActorLocation();

	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_AIMovement::StaticClass()))
	{
		II_AIMovement* _interface = Cast<II_AIMovement>(Owner);
		bool result = _interface->Execute_CustomMoveStart(Owner, Location);
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
void UBTT_MoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory,DeltaSeconds);
	ACharacter* Owner = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	ACharacter* Target = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(MoveToTarget.SelectedKeyName));

	if (!Target || !Owner)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector Location = Target->GetActorLocation();
	II_AIMovement* _interface = Cast<II_AIMovement>(Owner);

	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_AIMovement::StaticClass()))
	{
		bool Finish = _interface->Execute_CustomMoveTo(Owner, Location);
		if (Finish)
		{
			_interface->Execute_CustomMoveEnd(Owner, Location);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		_interface->Execute_CustomMoveEnd(Owner, Location);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	float Distance = FVector::Distance(Target->GetActorLocation(),Owner->GetActorLocation());
	float Radius = Owner->GetCapsuleComponent()->GetScaledCapsuleRadius() + Target->GetCapsuleComponent()->GetScaledCapsuleRadius();
	Distance -= Radius;

	if (Distance <= OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsFloat(AllowableRange.SelectedKeyName))
	{
		_interface->Execute_CustomMoveEnd(Owner, Location);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
EBTNodeResult::Type UBTT_MoveToTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();
	AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(MoveToTarget.SelectedKeyName));
	FVector Location = Target->GetActorLocation();

	II_AIMovement* _interface = Cast<II_AIMovement>(Owner);

	if (UKismetSystemLibrary::DoesImplementInterface(Owner, UI_AIMovement::StaticClass()))
	{
		bool Finish = _interface->Execute_CustomMoveEnd(Owner, Location);
	}
	return EBTNodeResult::Succeeded;
}