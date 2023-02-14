// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/I_CombatInteraction.h"
#include "../Global/GlobalCombat.h"

#include "EA_MasterCharacter.generated.h"

UCLASS()
class EA_UNREAL_C_API AEA_MasterCharacter : public ACharacter, public II_CombatInteraction
{
	GENERATED_BODY()

public:
	AEA_MasterCharacter();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
protected:
	FCharacterStat CharacterStat;
public:
	UFUNCTION(BlueprintCallable)
	void CharacterSetter(FName CharacterName,UAnimMontage* EquipMontage,UAnimMontage* DodgeMontage);
	UFUNCTION(BlueprintCallable)
	float CharacterTakeDamage(float Damage);


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
	void CatchAction(const struct FInputActionValue& Value);
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		bool IsCatchingAttack = false;
protected:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION(BlueprintCallable)
	void SetAttackMontages(UAnimMontage* Normal, UAnimMontage* Back, UAnimMontage* Loop, UAnimMontage* Air, TArray<UAnimMontage*> Catch, UAnimMontage* Hit);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<UAnimMontage> AM_NormalHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* AM_NormalAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* AM_BackAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* AM_LoopAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* AM_AirAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TArray<UAnimMontage*> AM_CatchingAttack;
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAttacking();
#pragma endregion
#pragma region Animation
protected: /* Animation Parameter */
	UPROPERTY(BlueprintReadOnly, Category = Animation)
		class UEA_MasterAnimInstance* MasterAnimInstance;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AM_Equip;

protected:
	UFUNCTION()
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
#pragma region Interface
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
		void PlayKnockBack();
	void PlayKnockBack_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
		void PlayStiffen();
	void PlayStiffen_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	bool PlayCatchAttack(UAnimMontage* montage, FName sectionName);
	bool PlayCatchAttack_Implementation(UAnimMontage* montage, FName sectionName);
#pragma endregion
};

#define IMC_Movement MovementMappingContext
#define IMC_Combat CombatMappingContext
#define AnimInstance MasterAnimInstance

#define StiffenPower 0.001f
#define StiffenTime 0.1f