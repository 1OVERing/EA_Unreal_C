#include "NS_CatchAttack.h"
#include "Kismet/GameplayStatics.h"
#include "../Interface/I_CombatInteraction.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "../Global/GlobalCombat.h"

void UNS_CatchAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	ECollisionChannel TraceChannel;
	(Cast<ACharacter>(MeshComp->GetOwner()) == UGameplayStatics::GetPlayerCharacter(MeshComp, 0)) ?
		TraceChannel = PlayerTraceChannel : TraceChannel = EnemyTraceChannel;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	FVector TraceStart = Owner->GetActorLocation();
	FVector TraceEnd = TraceStart + (Owner->GetActorForwardVector() * TraceDistance);
	TArray<AActor*> Temp;
	FHitResult Hit;
	if (UKismetSystemLibrary::CapsuleTraceSingle(MeshComp, TraceStart, TraceEnd, 25.f, 80.f, UEngineTypes::ConvertToTraceType(TraceChannel), false, Temp, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Red, FLinearColor::Green, 0.1f))
	{
		II_CombatInteraction* _interface = Cast<II_CombatInteraction>(Hit.GetActor());
		if (!_interface->Execute_PlayCatchAttack(Hit.GetActor(), CatchingAttackMontage, FName("Target"))) return;
		
		_interface = Cast<II_CombatInteraction>(Owner);
		if (!_interface->Execute_PlayCatchAttack(Owner, CatchingAttackMontage, FName("Owner"))) return;

		Hit.GetActor()->SetActorLocation(Owner->GetActorLocation() + (Owner->GetActorForwardVector() * AttachedDistance));

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Hit.GetActor()->GetActorLocation(), Owner->GetActorLocation());
		Hit.GetActor()->SetActorRotation(LookAtRotation);
	}

}