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
	FSkillSet():Name(FName()), AllowableMinRange(0.f), AllowableMaxRange(0.f),RechargeTime(0.f),RechargeTimeFinish(true) {}
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UAnimMontage> montage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AllowableMinRange;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float AllowableMaxRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RechargeTimeFinish;
};