// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EA_MasterAnimInstance.generated.h"

UCLASS()
class EA_UNREAL_C_API UEA_MasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UEA_MasterAnimInstance();
	virtual void NativeBeginPlay()override;
	virtual void NativeUpdateAnimation(float deltaSeconds)override;
#pragma region Movement
public:
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE void SetMovementScale(const FVector2D& scale);
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE void SetSprint(bool sprint) { IsSprint = sprint; }
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE void SetMovementAnimRate(float Rate) { MovementAnimRate = Rate; }
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE void SetFalling(bool Falling) { IsFalling = Falling; }
	UFUNCTION(BlueprintCallable, Category = Movement)
		bool PlayJumping(FVector LaunchDirection);
	UFUNCTION()
		void AnimNotify_Jumping();
	UFUNCTION(Category = Movement)
		void LandedEvent(const FHitResult& Hit);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		FVector2D MovementScale = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		FVector2D PrevMovementScale = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		bool IsSprint = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		float MovementAnimRate = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		float SprintTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool Jumping = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		FVector JumpDirection = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool IsFalling = false;

#pragma endregion
#pragma region Combat
public:
	UFUNCTION(BlueprintCallable)
		void SetCombatMode(bool mode);
	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetCombatMode();
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetCurremtMovementIsSprint();
	UFUNCTION()
		void AnimNotify_NextAttackCheck();
		void SetNextAttack();
		void EndedAttack();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool IsNextAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		bool CombatMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		float MaxSprintTime = 2.f;
#pragma endregion
};