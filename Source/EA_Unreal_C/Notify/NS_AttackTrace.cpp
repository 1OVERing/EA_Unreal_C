// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_AttackTrace.h"
#include "Gameframework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagarafunctionLibrary.h"
#include "../Interface/I_CombatInteraction.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Global/GlobalCombat.h"


void UNS_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	TraceLocations.Empty();
	TraceLocations.Add(FVector(0.f));
	TraceLocations.Add(FVector(0.f));
	TraceLocations.Add(FVector(0.f));
	TraceLocations.Add(FVector(0.f));
}
void UNS_AttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	auto Owner = Cast<ACharacter>(MeshComp->GetOwner());
	/* 애니메이션 시퀸스 디버그용 */
	if (!Owner)
	{
		bool result = false;
		SetTraceLocation(MeshComp);
		if (Trace(MeshComp).IsEmpty()) return;
		return;
	}

	SetTraceLocation(MeshComp);
	TMap<AActor*, FHitResult> HittedActors = Trace(MeshComp);
	for (auto& value : HittedActors)
	{
		AActor* TargetActor = value.Value.GetActor();
		HittedActor.AddUnique(TargetActor);


		/* PlayEffect */
		if (HittedEffect)UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), HittedEffect, value.Value.ImpactPoint);

		ACharacter* targetCharacter = Cast<ACharacter>(TargetActor);
		/* Knockback */
		if (IsKnockback)
		{
			if (targetCharacter)
			{
				FVector KnockbackPower = Owner->GetActorForwardVector() * (KnockbackDistance * 1000.f);
				KnockbackPower.Z = 0.f;
				targetCharacter->LaunchCharacter(KnockbackPower, false, false);
			}
		}

		{ // CombatInteraction
			II_CombatInteraction* _interface = Cast<II_CombatInteraction>(TargetActor);
			if (IsKnockback) _interface->Execute_PlayKnockBack(TargetActor);
		}
		{
			II_CombatInteraction* _interface = Cast<II_CombatInteraction>(Owner);
			if(IsStiffen) _interface->Execute_PlayStiffen(Owner);
		}
		{
			UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, Owner->GetController(), Owner, UDamageType::StaticClass());
		}
		{
			if (MontageLookAt)
			{
				FRotator rotation = UKismetMathLibrary::FindLookAtRotation(targetCharacter->GetActorLocation(), Owner->GetActorLocation());
				targetCharacter->SetActorRotation(rotation);
			}



			if (TargetMontage) targetCharacter->PlayAnimMontage(TargetMontage);
		}
	}

}
void UNS_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HittedActor.Empty();
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void UNS_AttackTrace::AddHittedActor(bool Condition, UPARAM(ref)TArray<FHitResult>& Array) const
{
}
TMap<AActor*, FHitResult> UNS_AttackTrace::Trace(const UMeshComponent* MeshComp)
{
	TMap<AActor*, FHitResult> SaveOutHit;
	RealTrace(MeshComp, SaveOutHit, 0, 1);
	RealTrace(MeshComp, SaveOutHit, 0, 2);
	RealTrace(MeshComp, SaveOutHit, 0, 3);
	RealTrace(MeshComp, SaveOutHit, 1, 2);
	RealTrace(MeshComp, SaveOutHit, 1, 3);
	return SaveOutHit;
}
void UNS_AttackTrace::RealTrace(const UMeshComponent* MeshComp, TMap<AActor*, FHitResult>& SaveOutHit, int FirstIndex, int SecondIndex)
{
	TArray<FHitResult> hits;
	ECollisionChannel TraceChannel = (TraceType == ECurrentTrace::Player) ? PlayerTraceChannel : EnemyTraceChannel;
	DrawDebugLine(MeshComp->GetWorld(), TraceLocations[FirstIndex], TraceLocations[SecondIndex], FColor::Red, false, TraceDrawTime);
	FCollisionQueryParams Traceparam;
	Traceparam.AddIgnoredActors(HittedActor);
	if (MeshComp->GetWorld()->LineTraceMultiByChannel(hits, TraceLocations[FirstIndex], TraceLocations[SecondIndex], TraceChannel, Traceparam))
	{
		for (auto hit : hits)
		{
			if (!SaveOutHit.Find(hit.GetActor()))
			{
				SaveOutHit.Emplace(hit.GetActor(), hit);
				HittedActor.AddUnique(hit.GetActor());
			}
		}
	}
}
void UNS_AttackTrace::SetTraceLocation(const UMeshComponent* MeshComp)
{
	if (TraceLocations[0].IsZero() && TraceLocations[1].IsZero())
	{
		TraceLocations[0] = MeshComp->GetSocketLocation(TEXT("Twinblades_L_End"));
		TraceLocations[1] = MeshComp->GetSocketLocation(TEXT("Twinblades_R_End"));
	}

	TraceLocations[2] = TraceLocations[0];
	TraceLocations[3] = TraceLocations[1];

	TraceLocations[0] = MeshComp->GetSocketLocation(TEXT("Twinblades_L_End"));
	TraceLocations[1] = MeshComp->GetSocketLocation(TEXT("Twinblades_R_End"));
}
