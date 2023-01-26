#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NS_CatchAttack.generated.h"

UCLASS()
class EA_UNREAL_C_API UNS_CatchAttack : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TraceDistance = 80.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAnimMontage> CatchingAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector TraceLocationOffset = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector TraceLocationOffset_End = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AttachedDistance = 50.f;
};
