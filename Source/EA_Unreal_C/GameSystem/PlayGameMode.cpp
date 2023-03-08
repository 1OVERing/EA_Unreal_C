// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GamePlayStatics.h"

void APlayGameMode::PlayerDeath_Implementation()
{
	if (PlayerDeathWidget)
	{
		UUserWidget* widget =  CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerDeathWidget);
		widget->AddToViewport(0);
	}
}
