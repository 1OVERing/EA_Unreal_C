// Fill out your copyright notice in the Description page of Project Settings.


#include "N_TickAttack.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "../Global/GlobalCombat.h"


void UN_TickAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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
		Hit.GetActor()->SetActorLocation(Owner->GetActorLocation() + (Owner->GetActorForwardVector() * AttachedDistance));
		UGameplayStatics::ApplyDamage(Hit.GetActor(), AttackDamage,nullptr,nullptr,nullptr);

		/* PlayEffect */
		if (HittedEffect)UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), HittedEffect, Hit.ImpactPoint);
	}
	
}
