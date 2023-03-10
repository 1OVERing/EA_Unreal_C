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

	if (LookAtPlayer && ::IsValid(MeshComp->GetOwner()))
	{
		ACharacter* player = UGameplayStatics::GetPlayerCharacter(MeshComp, 0);
		ACharacter* Owner = Cast<ACharacter>(MeshComp->GetOwner());
		if (::IsValid(player) && ::IsValid(Owner))
		{
			float LookAtYaw = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), player->GetActorLocation()).Yaw;
			Owner->SetActorRotation(FRotator(Owner->GetActorRotation().Pitch, LookAtYaw, Owner->GetActorRotation().Roll));
		}
	}
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

		float Damage = UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, Owner->GetController(), Owner, UDamageType::StaticClass());

		if (Damage > 0)
		{
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

			II_CombatInteraction* Owner_interface = Cast<II_CombatInteraction>(Owner);
			II_CombatInteraction* Target_interface = Cast<II_CombatInteraction>(TargetActor);
			if (IsStiffen) Owner_interface->Execute_PlayStiffen(Owner);
			if (IsKnockback) Target_interface->Execute_PlayKnockBack(TargetActor);
			if (MontageLookAt)
			{
				float LookAt = UKismetMathLibrary::FindLookAtRotation(targetCharacter->GetActorLocation(), Owner->GetActorLocation()).Yaw;
				targetCharacter->SetActorRotation(FRotator(targetCharacter->GetActorRotation().Pitch, LookAt, targetCharacter->GetActorRotation().Roll));
			}
			if (TargetMontage) Target_interface->Execute_PlayCatchAttack(TargetActor,TargetMontage,FName());
		}
		else if (Damage == -1)
		{
			Owner->GetMesh()->GetAnimInstance()->Montage_JumpToSection("Parrying");
		}
	}

}
void UNS_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HittedActor.Empty();
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
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
	ETraceTypeQuery TraceQuery = UEngineTypes::ConvertToTraceType(TraceChannel);
	switch (TraceShape)
	{
	case ETraceShape::Line:
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
		break;
	case ETraceShape::Sphere:
		if (UKismetSystemLibrary::SphereTraceMulti(MeshComp, TraceLocations[FirstIndex], TraceLocations[SecondIndex], SphereRadius, TraceQuery, false, HittedActor, DebugTraceDraw, hits, true, FLinearColor::Red, FLinearColor::Green, 0.1f))
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
		break;
	default:
		break;
	}
}
void UNS_AttackTrace::SetTraceLocation(const UMeshComponent* MeshComp)
{
	FName TraceStart;
	FName TraceEnd;
	switch (WeaponType)
	{
	case EWeaponType::Twinblades:
		TraceStart = "Twinblades_L_End";
		TraceEnd = "Twinblades_R_End";
		break;
	case EWeaponType::Katana:
		TraceStart = "Katana_Start";
		TraceEnd = "Katana_End";
		break;
	default:
		break;
	}
	if (TraceLocations[0].IsZero() && TraceLocations[1].IsZero())
	{
		TraceLocations[0] = MeshComp->GetSocketLocation(TraceStart);
		TraceLocations[1] = MeshComp->GetSocketLocation(TraceEnd);
	}

	TraceLocations[2] = TraceLocations[0];
	TraceLocations[3] = TraceLocations[1];

	TraceLocations[0] = MeshComp->GetSocketLocation(TraceStart);
	TraceLocations[1] = MeshComp->GetSocketLocation(TraceEnd);
}
