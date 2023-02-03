#include "AIC_MasterEnemy.h"

#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "UObject/ConstructorHelpers.h"

AAIC_MasterEnemy::AAIC_MasterEnemy(FObjectInitializer const& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> Obj(TEXT("/Game/Blueprints/AI/BT_MasterEnemy.BT_MasterEnemy"));
	if (Obj.Succeeded()) m_BT = Obj.Object;

	BehaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this,TEXT("BehaviorTreeComponent"));
	m_BB = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));

	SetupPerceptionSystem();
}

void AAIC_MasterEnemy::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(m_BT);
	BehaviorTreeComponent->StartTree(*m_BT);

}

void AAIC_MasterEnemy::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);

	if (m_BB)m_BB->InitializeBlackboard(*m_BT->BlackboardAsset);

	SetBlackboardOriginLocation(GetPawn()->GetActorLocation());
}
UBlackboardComponent* AAIC_MasterEnemy::GetBlackboard() {return m_BB;}

void AAIC_MasterEnemy::SetBlackboardOriginLocation(const FVector& origin)
{
	GetBlackboard()->SetValueAsVector("OriginLocation", origin);
}
FVector AAIC_MasterEnemy::GetBlackboardOriginLocation()
{
	return GetBlackboard()->GetValueAsVector("OriginLocation");
}

void AAIC_MasterEnemy::Perception_Updated(AActor* UpdatedActor,FAIStimulus const stimulus)
{
	if(stimulus.WasSuccessfullySensed()) GEngine->AddOnScreenDebugMessage(1233, 1.f, FColor::Red, TEXT("Success"));
	else GEngine->AddOnScreenDebugMessage(1233, 1.f, FColor::Red, TEXT("NotSuccess"));
}

void AAIC_MasterEnemy::SetupPerceptionSystem()
{
	AIPerceptionSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));

	AIPerceptionSight->SightRadius = 500.f;
	AIPerceptionSight->LoseSightRadius = AIPerceptionSight->SightRadius + 250.f;
	AIPerceptionSight->PeripheralVisionAngleDegrees = 75.f;
	AIPerceptionSight->SetMaxAge(0.f);
	AIPerceptionSight->AutoSuccessRangeFromLastSeenLocation = 900.f;
	AIPerceptionSight->DetectionByAffiliation.bDetectFriendlies = false;
	AIPerceptionSight->DetectionByAffiliation.bDetectNeutrals = true;
	AIPerceptionSight->DetectionByAffiliation.bDetectEnemies = true;

	GetPerceptionComponent()->SetDominantSense(AIPerceptionSight->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,&AAIC_MasterEnemy::Perception_Updated);
	GetPerceptionComponent()->ConfigureSense(*AIPerceptionSight);
}
