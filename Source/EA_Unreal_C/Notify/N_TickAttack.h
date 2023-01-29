// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NiagaraSystem.h"
#include "N_TickAttack.generated.h"

UCLASS()
class EA_UNREAL_C_API UN_TickAttack : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AttackDamage = 1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> HittedEffect;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TraceDistance = 80.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AttachedDistance = 50.f;
};
