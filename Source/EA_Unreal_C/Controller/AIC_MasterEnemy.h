#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIC_MasterEnemy.generated.h"

UCLASS()
class EA_UNREAL_C_API AAIC_MasterEnemy : public AAIController
{
	GENERATED_BODY()
public:
	AAIC_MasterEnemy();
	void BeginPlay()override;
	void OnPossess(APawn* const pawn) override;
public:
	class UBlackboardComponent* GetBlackboard();


protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Perception")
	class UAISenseConfig_Sight* AIPerceptionSight;

	UFUNCTION()
	void Perception_Updated(AActor* UpdatedActor, FAIStimulus const stimulus);
	void SetupPerceptionSystem();
};
