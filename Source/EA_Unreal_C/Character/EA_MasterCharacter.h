// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EA_MasterCharacter.generated.h"

UCLASS()
class EA_UNREAL_C_API AEA_MasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEA_MasterCharacter();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void CharacterSetter(FName CharacterName,UAnimMontage* EquipMontage,UAnimMontage* DodgeMontage);
#pragma region InputSystem
protected: /* Input System Parameter*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = EnhancedInputSystem)
	class UIMC_Movement* MovementMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnhancedInputSystem)
	class UIMC_Combat* CombatMappingContext;
protected: /* Input System Bind Function*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveAction_Triggered(const struct FInputActionValue& Value);
	void MoveAction_Completed(const struct FInputActionValue& Value);
	void LookAction(const struct FInputActionValue& Value);
	void SprintAction(const struct FInputActionValue& Value);
	void JumpAction(const struct FInputActionValue& Value);
	void EquipAction(const struct FInputActionValue& Value);
	void LMouseAction(const struct FInputActionValue& Value);
	void RMouseAction(const struct FInputActionValue& Value);
#pragma endregion
#pragma region Movement
protected: /* Movement Parameter */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool IsSprint = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool IsJumping = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AM_Dodge;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		FVector2D MovementScale = FVector2D::ZeroVector;
protected:/* Movement Function */
	UFUNCTION()
		void LandedEvent(const FHitResult& Hit);
#pragma endregion
#pragma region Combat
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		bool IsEquip = false;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* NormalAttackCombo;
#pragma endregion
#pragma region Animation
protected: /* Animation Parameter */
	UPROPERTY(BlueprintReadOnly, Category = Animation)
		class UEA_MasterAnimInstance* MasterAnimInstance;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AM_Equip;

protected:
	void EndedMontage(UAnimMontage* Montage, bool bInterrupted);

#pragma endregion
#pragma region Camera
protected: /* Camera Parameter*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraArm; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma endregion
};

#define IMC_Movement MovementMappingContext
#define IMC_Combat CombatMappingContext
#define AnimInstance MasterAnimInstance