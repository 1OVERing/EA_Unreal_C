#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "../Interface/I_AIMovement.h"
#include "../Interface/I_CombatInteraction.h"

#include "EA_MasterEnemy.generated.h"

UCLASS()
class EA_UNREAL_C_API AEA_MasterEnemy : public ACharacter , public II_AIMovement,public II_CombatInteraction
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
	TObjectPtr<UAnimMontage> AM_TrunL90;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UAnimMontage> AM_TrunL180;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UAnimMontage> AM_TrunR90;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UAnimMontage> AM_TrunR180;
public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE void SetMontages_Trun(const UAnimMontage* am_L90,const UAnimMontage* am_L180, const UAnimMontage* am_R90,const UAnimMontage* am_R180);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool FindWaypoint(const FVector MoveToLocation);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool MoveCheck();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Movement")
	FORCEINLINE bool InRotation();
	const FVector GetNextMovePoint();
#pragma endregion
#pragma region Combat
protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
		FTimerHandle HitTimer;
public:
	UFUNCTION(BlueprintPure,BlueprintCallable, Category = "Combat")
	FORCEINLINE bool IsHitReaction();
#pragma endregion
#pragma region Animtion
protected:
	TObjectPtr<class UEA_MasterAnimInstance> AnimInstance;
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
