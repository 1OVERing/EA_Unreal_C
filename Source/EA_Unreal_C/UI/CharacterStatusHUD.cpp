// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusHUD.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterStatusHUD::SetHealth(float CurHealth, float MaxHealth)
{
	if (HealthBar) HealthBar->SetPercent(CurHealth / MaxHealth);
}

void UCharacterStatusHUD::SetStamina(float CurStamina, float MaxStamina)
{
	if (StaminaBar) StaminaBar->SetPercent(CurStamina / MaxStamina);
}

void UCharacterStatusHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!OwnerActor) return;
	FVector Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	FVector CurLocation = OwnerActor->GetActorLocation();
	float TargetDistance = FVector::Distance(Player, CurLocation);

	if (TargetDistance > 1000.f)
	{
		this->SetRenderOpacity(0.f);
	}
	else
	{
		this->SetRenderOpacity(UKismetMathLibrary::FClamp((UKismetMathLibrary::Abs((TargetDistance / 1000.f) - 1) * 2.f),0,1));
	}
}
