// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NS_RootmotionAutoMoveY.generated.h"

/**
 * 
 */
UCLASS()
class EA_UNREAL_C_API UNS_RootmotionAutoMoveY : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MoveScale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool BackMove = false;
};
