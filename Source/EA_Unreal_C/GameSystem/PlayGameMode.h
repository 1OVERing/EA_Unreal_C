// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayGameMode.generated.h"

UCLASS()
class EA_UNREAL_C_API APlayGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void PlayerDeath();
	void PlayerDeath_Implementation();

	UPROPERTY(EditAnywhere)
		class TSubclassOf<UUserWidget> PlayerDeathWidget;
};
