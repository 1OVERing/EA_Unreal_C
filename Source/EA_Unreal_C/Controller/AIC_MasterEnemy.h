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
	AAIC_MasterEnemy(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());
	void BeginPlay()override;
	void OnPossess(APawn* const pawn) override;
public:
	class UBlackboardComponent* GetBlackboard();

	void SetBlackboardOriginLocation(const FVector& origin);
	FVector GetBlackboardOriginLocation();
private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* m_BT;
	class UBlackboardComponent* m_BB;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Perception")
	class UAISenseConfig_Sight* AIPerceptionSight;

	UFUNCTION()
	void Perception_Updated(AActor* UpdatedActor, FAIStimulus const stimulus);
	void SetupPerceptionSystem();
};
