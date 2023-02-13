#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "../Interface/I_AIMovement.h"
#include "../Interface/I_CombatInteraction.h"

#include "EA_MasterEnemy.generated.h"

UCLASS()
class EA_UNREAL_C_API AEA_MasterEnemy : public ACharacter, public II_AIMovement, public II_CombatInteraction
{
	GENERATED_BODY()
public:
	AEA_MasterEnemy();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
#pragma region Character
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Character")
		TObjectPtr<USkeletalMeshComponent> SK_Weapon;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Character")
		TObjectPtr<class AAIC_MasterEnemy> EnemyController;
#pragma endregion
#pragma region Movement
protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		TArray<FVector> WayPoints;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		FVector PrevLocation;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		double StopLocationTime;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		TObjectPtr<UAnimMontage> AM_TrunL180;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		TObjectPtr<UAnimMontage> AM_TrunR180;
public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
		FORCEINLINE void SetMontages_Trun(UAnimMontage* am_L180, UAnimMontage* am_R180);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		bool FindWaypoint(const FVector MoveToLocation);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		bool MoveCheck();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Movement")
		FORCEINLINE bool InRotation();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		FORCEINLINE void SetSprint(bool spint);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		const FVector GetNextMovePoint();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void SprintCheck();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Movement")
		float FindAngle(const FVector TargetLocation);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void PlayRotationMontage(FVector2D Angle);
	
	bool RotationCheck(const FVector& TargetLocation);
	FORCEINLINE bool DistanceCheck(const FVector& TargetLocation);
	void SetRotation(const FVector& TargetLocation);
	void SetMove(const FVector& TargetLocation);
#pragma endregion
#pragma region Combat
protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
		FTimerHandle HitTimer;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
		TObjectPtr<UAnimMontage> AM_Hit_Forward;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
		TObjectPtr<UAnimMontage> AM_Hit_Backward;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
		TObjectPtr<UAnimMontage> AM_Hit_Right;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
		TObjectPtr<UAnimMontage> AM_Hit_Left;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AM_Equip;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
		TArray<struct FSkillSet> SkillSet;
	int CurrentSkillIndex = -1;
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
		FORCEINLINE void SetMontages_Hit(UAnimMontage* Forward, UAnimMontage* Backward, UAnimMontage* Right, UAnimMontage* Left);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE void SetMontages_Attacks(UAnimMontage* Equip,TArray<struct FSkillSet> Skills);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetTargetDistance();
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void WeaponEquip(bool equip);
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void PlayHitAnimMontage(const AActor* Causer);
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Combat")
		FORCEINLINE bool IsHitReaction();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
		void SightTarget(AActor* SightTarget);
		void SightTarget_Implementation(AActor* SightTarget);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
		void HitEndTimerFunc();
		void HitEndTimerFunc_Implementation();
		void SetHitTimer(float Time);
		UFUNCTION()
		void ResetSkillRecharge(int SkillIndex);
	UFUNCTION(BlueprintCallable, Category = "Combat")
		FORCEINLINE bool IsHit();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Combat")
		FORCEINLINE bool CanHit();
#pragma endregion
#pragma region Animtion
protected:
	TObjectPtr<class UEA_MasterAnimInstance> AnimInstance;

	UFUNCTION()
	void MontageBledOut(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion

#pragma region Interface_CombatInteraction
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void PlayKnockBack();
	void PlayKnockBack_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void PlayStiffen();
	void PlayStiffen_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool PlayCatchAttack(UAnimMontage* montage, FName sectionName);
	bool PlayCatchAttack_Implementation(UAnimMontage* montage, FName sectionName);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool SetNextAttack();
	bool SetNextAttack_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		float PlayAttack();
	float PlayAttack_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool AttackEndCheck();
	bool AttackEndCheck_Implementation();

#pragma endregion
#pragma region Interface_AIMove
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
		bool CustomMoveStart(FVector MoveToLocation);
	bool CustomMoveStart_Implementation(FVector MoveToLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
		bool CustomMoveTo(FVector MoveToLocation);
	bool CustomMoveTo_Implementation(FVector MoveToLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
		bool CustomMoveEnd(FVector MoveToLocation);
	bool CustomMoveEnd_Implementation(FVector MoveToLocation);
#pragma endregion
};
