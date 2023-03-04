// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusHUD.h"
#include "Components/ProgressBar.h"

void UCharacterStatusHUD::SetHealth(float CurHealth, float MaxHealth)
{
	if (HealthBar) HealthBar->SetPercent(CurHealth / MaxHealth);
}

void UCharacterStatusHUD::SetStamina(float CurStamina, float MaxStamina)
{
	if (StaminaBar) StaminaBar->SetPercent(CurStamina / MaxStamina);
}
