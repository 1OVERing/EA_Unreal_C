// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NiagaraSystem.h"
#include "NS_AttackTrace.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None UMETA(DisplayName = "None"),
	Twinblades UMETA(DisplayName = "Twinblades"),
};

UCLASS(Blueprintable, BlueprintType)
class UNS_AttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)override;
public:
	UFUNCTION(BlueprintCallable)
		void AddHittedActor(bool Condition, UPARAM(ref) TArray<FHitResult>& Array) const;

	void SetTraceLocation(const UMeshComponent* MeshComp);
	TMap<AActor*, FHitResult> Trace(const UMeshComponent* MeshComp);
	void RealTrace(const UMeshComponent* MeshComp, TMap<AActor*, FHitResult>& SaveOutHit,int FirstIndex,int SecondIndex);
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AttackDamage =1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EWeaponType WeaponType;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FVector> TraceLocations;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<AActor*> HittedActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsKnockback = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float KnockbackDistance = 1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TObjectPtr<UNiagaraSystem> HittedEffect;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsStiffen = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* TargetMontage = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool MontageLookAt = false;
};
