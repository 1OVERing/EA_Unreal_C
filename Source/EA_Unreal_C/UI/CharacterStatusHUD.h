// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusHUD.generated.h"

/**
 * 
 */
UCLASS()
class EA_UNREAL_C_API UCharacterStatusHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealth(float CurHealth, float MaxHealth);
	void SetStamina(float CurStamina, float MaxStamina);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UPROPERTY(EditAnywhere)
		TObjectPtr<AActor> OwnerActor;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* HealthBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* StaminaBar;
};
