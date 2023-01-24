// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_AttackTrace.h"
#include "Gameframework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagarafunctionLibrary.h"
#include "../Interface/I_CombatInteraction.h"

#define TraceDrawTime 0.1f

#define CurrentTraceChannel ECC_GameTraceChannel1

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
	/* �ִϸ��̼� ������ ����׿� */
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

		/* Knockback */
		if (IsKnockback)
		{
			ACharacter* targetCharacter = Cast<ACharacter>(TargetActor);
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
			if(IsStiffen) _interface->Execute_PlayStiffen(TargetActor);
		}


		{
			UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, Owner->GetController(), Owner, UDamageType::StaticClass());
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

	DrawDebugLine(MeshComp->GetWorld(), TraceLocations[FirstIndex], TraceLocations[SecondIndex], FColor::Red, false, TraceDrawTime);
	FCollisionQueryParams Traceparam;
	Traceparam.AddIgnoredActors(HittedActor);
	if (MeshComp->GetWorld()->LineTraceMultiByChannel(hits, TraceLocations[FirstIndex], TraceLocations[SecondIndex], CurrentTraceChannel, Traceparam))
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