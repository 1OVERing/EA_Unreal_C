#pragma once
#include "EngineMinimal.h"
#include "GlobalCombat.generated.h"

#define PlayerTraceChannel ECC_GameTraceChannel1
#define EnemyTraceChannel ECC_GameTraceChannel2

#define TraceDrawTime 0.1f

USTRUCT(BlueprintType)
struct FSkillSet
{
	GENERATED_BODY()
public:
	FSkillSet():Name(FName()), AllowableRange(0.f) {}
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UAnimMontage> montage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float AllowableRange;
};