#pragma once
#include "EngineMinimal.h"
#include "GlobalCombat.generated.h"

#define PlayerTraceChannel ECC_GameTraceChannel1
#define EnemyTraceChannel ECC_GameTraceChannel2


#define GuardHealingSpeed 1.f
#define DebugTraceDraw EDrawDebugTrace::ForDuration
#define TraceDrawTime 0.1f

USTRUCT(BlueprintType)
struct FCharacterStat
{
	GENERATED_BODY()
public:
	FCharacterStat():CharacterName(),MaxHP(10.f),MaxStamina(100.f),CurHP(10.f),CurStamina(10.f){}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CharacterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurStamina;
public:
	void ResetCharacterStat();
	float TakeDamage(float Damage);
	float TakeStaminaPoint(float Damage);
};

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