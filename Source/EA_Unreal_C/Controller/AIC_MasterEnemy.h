#pragma once

#include "EngineMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIC_MasterEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSightTarget, AActor*, SightActor);

UCLASS()
class EA_UNREAL_C_API AAIC_MasterEnemy : public AAIController
{
	GENERATED_BODY()
public:
	AAIC_MasterEnemy(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());
	void BeginPlay()override;
	void OnPossess(APawn* const pawn) override;

public:
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "Perception")
	FSightTarget OnSightTarget;
#pragma region Blackboard
public:
	UFUNCTION(BlueprintCallable, Category = "BlackBoard")
		FORCEINLINE class UBlackboardComponent* GetBlackboard();
	UFUNCTION(BlueprintCallable, Category = "BlackBoard")
		FORCEINLINE void SetBlackboardOriginLocation(const FVector& origin);
	UFUNCTION(BlueprintCallable, Category = "BlackBoard")
		FORCEINLINE FVector GetBlackboardOriginLocation();
	UFUNCTION(BlueprintCallable, Category = "BlackBoard")
		FORCEINLINE AActor* GetBB_TargetActor();
	UFUNCTION(BlueprintCallable, Category = "BlackBoard")
		FORCEINLINE void SetBB_TargetActor(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "BlackBoard")
		FORCEINLINE void SetBB_AllowableRange(float allowablerange);
#pragma endregion
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
