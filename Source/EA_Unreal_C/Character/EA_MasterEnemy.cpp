#include "EA_MasterEnemy.h"
#include "../Controller/AIC_MasterEnemy.h"
#include "../Character/EA_MasterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AEA_MasterEnemy::AEA_MasterEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
#pragma region MeshSetting

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Body(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Combines/SK_PP801P3_E.SK_PP801P3_E"));
	if (Body.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Body.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Katana(TEXT("/Game/Meshes/Weapon/Katana/SK_Katana.SK_Katana"));
	if (Katana.Succeeded())
	{
		SK_Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_Katana"));
		SK_Weapon->SetSkeletalMesh(Katana.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK_Weapon->AttachToComponent(GetMesh(), Rules, FName("KatanaSheath"));
	}
#pragma endregion
#pragma region Animation
	ConstructorHelpers::FClassFinder<UEA_MasterAnimInstance> Instance(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/AnimationBlueprints/ABP_MasterEnemy.ABP_MasterEnemy_C'"));
	if (Instance.Succeeded())
	{
		GetMesh()->SetAnimClass(Instance.Class);
	}
#pragma endregion
#pragma region Controller
	this->AIControllerClass = AAIC_MasterEnemy::StaticClass();
#pragma endregion
}
void AEA_MasterEnemy::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UEA_MasterAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyController = Cast<AAIC_MasterEnemy>(GetController());
	if (!AnimInstance || !EnemyController)
	{
		GEngine->AddOnScreenDebugMessage(999, 1.f, FColor::Red, TEXT("Failed Create AnimInstance & Controller (AEA_MasterEnemy::BeginPlay)"));
		this->Destroy();
	}
}
void AEA_MasterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEA_MasterEnemy::SetMontages_Trun(const UAnimMontage* am_L90, const  UAnimMontage* am_L180, const  UAnimMontage* am_R90, const  UAnimMontage* am_R180)
{
	AM_TrunL90 = am_L90;
	AM_TrunL180 = am_L180;
	AM_TrunR90 = am_R90;
	AM_TrunR180 = am_R180;
}

bool AEA_MasterEnemy::FindWaypoint(const FVector MoveToLocation)
{
	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, GetActorLocation(), MoveToLocation);
	if (0 >= Path->GetPathLength()) return false;

	WayPoints = Path->PathPoints;
	return true;
}
bool AEA_MasterEnemy::MoveCheck()
{
	if (InRotation()) return true;

	if (GetActorLocation() != PrevLocation)
	{
		PrevLocation = GetActorLocation();
		StopLocationTime = 0.f;
		return true;
	}

	StopLocationTime += GetWorld()->GetDeltaSeconds();
	if (1.f < StopLocationTime) return false;

	return true;
}
bool AEA_MasterEnemy::InRotation()
{
	auto montage = this->GetCurrentMontage();
	return (montage == AM_TrunL90 || montage == AM_TrunL180 || montage == AM_TrunR90 || montage == AM_TrunR180);
}
const FVector AEA_MasterEnemy::GetNextMovePoint()
{
	if (WayPoints.IsEmpty()) return FVector::ZeroVector;
	float TargetDistance = UKismetMathLibrary::Distance2D(FVector2D(GetActorLocation().X, GetActorLocation().Y), FVector2D(WayPoints[0].X, WayPoints[0].Y));

	while (100.f > TargetDistance)
	{
		WayPoints.RemoveAt(0);
		if (WayPoints.IsEmpty()) return FVector::ZeroVector;
		TargetDistance = UKismetMathLibrary::Distance2D(FVector2D(GetActorLocation().X, GetActorLocation().Y), FVector2D(WayPoints[0].X, WayPoints[0].Y));
	}

	return WayPoints[0];
}
bool AEA_MasterEnemy::IsHitReaction()
{
	return HitTimer.IsValid();
}

#pragma region Interface_CombatInteraction
void AEA_MasterEnemy::SetSprint(bool spint)
{
	AnimInstance->SetSprint()
}
void AEA_MasterEnemy::PlayKnockBack_Implementation() {}
void AEA_MasterEnemy::PlayStiffen_Implementation() {}
bool AEA_MasterEnemy::PlayCatchAttack_Implementation(UAnimMontage* montage, FName sectionName) { return false; }
#pragma endregion
#pragma region Interface_AIMove
bool AEA_MasterEnemy::CustomMoveStart_Implementation(FVector MoveToLocation)
{
	if (!FindWaypoint(MoveToLocation)) return false;

	if (IsHitReaction())return false;

	return true;
}
bool AEA_MasterEnemy::CustomMoveTo_Implementation(FVector MoveToLocation)
{
	if (!MoveCheck()) return true;

	FVector NextWaypoint = GetNextMovePoint();
	if (NextWaypoint == FVector::ZeroVector) return true;

	if (AnimInstance->GetCombatMode())
	{// CombatMode Move

	}
	else
	{// Normalmode Move
		
	}

	return true;
}
bool AEA_MasterEnemy::CustomMoveEnd_Implementation(FVector MoveToLocation)
{
	AnimInstance->SetMovementScale(FVector2D::ZeroVector);
	AnimInstance->SetSprint(false);
	WayPoints.Empty();
	StopLocationTime = 0.f;
	return true;
}
#pragma endregion