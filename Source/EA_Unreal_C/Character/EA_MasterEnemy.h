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
#pragma endregion
#pragma region Movement
protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		TArray<FVector> WayPoints;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		FVector PrevLocation;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
		double StopLocationTime;
#pragma endregion
#pragma region Combat
protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
		FTimerHandle HitTimer;
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
	bool CustomMoveStart();
	bool CustomMoveStart_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
	bool CustomMoveTo();
	bool CustomMoveTo_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Move")
	bool CustomMoveEnd();
	bool CustomMoveEnd_Implementation();
#pragma endregion
};
