#include "EA_MasterEnemy.h"
#include "../Controller/AIC_MasterEnemy.h"
#include "../Character/EA_MasterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
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
	GetMesh()->SetCollisionProfileName("NoCollision");
	GetCapsuleComponent()->SetCollisionProfileName("EnemyCharacter");
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
	EnemyController->OnSightTarget.AddDynamic(this, &AEA_MasterEnemy::SightTarget);
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

void AEA_MasterEnemy::SetMontages_Trun(UAnimMontage* am_L90, UAnimMontage* am_L180, UAnimMontage* am_R90, UAnimMontage* am_R180)
{
	AM_TrunL90 = am_L90;
	AM_TrunL180 = am_L180;
	AM_TrunR90 = am_R90;
	AM_TrunR180 = am_R180;
}

#pragma region Combat
void AEA_MasterEnemy::SetMontages_Hit(UAnimMontage* Forward, UAnimMontage* Backward, UAnimMontage* Right, UAnimMontage* Left)
{
	AM_Hit_Forward = Forward;
	AM_Hit_Backward = Backward;
	AM_Hit_Right = Right;
	AM_Hit_Left = Left;
}
float AEA_MasterEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	PlayHitAnimMontage(DamageCauser);

	return 1.f;
}
void AEA_MasterEnemy::PlayHitAnimMontage(const AActor* Causer)
{
	{// 예외처리

	}

	StopAnimMontage();
	FVector2D Dir = FindVectorToDirection(Causer->GetActorLocation());
	float HitTime = 0.f;
	if (Dir.Y > 0.5f)
	{
		HitTime = PlayAnimMontage(AM_Hit_Forward);
	}
	else if (Dir.Y < -0.5f)
	{
		HitTime = PlayAnimMontage(AM_Hit_Backward);
	}
	else if (Dir.X > 0.5f)
	{
		HitTime = PlayAnimMontage(AM_Hit_Right);
	}
	else if (Dir.X < -0.5f)
	{
		HitTime = PlayAnimMontage(AM_Hit_Left);
	}
}
void AEA_MasterEnemy::SightTarget_Implementation(AActor* SightTarget)
{
	if(SightTarget) AnimInstance->SetCombatMode(true);
	else AnimInstance->SetCombatMode(false);


}
bool AEA_MasterEnemy::IsHitReaction()
{
	return HitTimer.IsValid();
}
#pragma endregion
#pragma region Movement
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
bool AEA_MasterEnemy::PlayCombatMove()
{
	AActor* Target = EnemyController->GetBB_TargetActor();

	FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
	float Angle = FindAngle(Target->GetActorLocation());

	if (!InRotation() && UKismetMathLibrary::Abs(Angle) <= 80.f)
	{// LookAtRotation
		this->SetActorRotation(FRotator(0.f,LookAtRotator.Yaw,0.f));
	}
	else if (!InRotation() && UKismetMathLibrary::Abs(Angle) > 80.f)
	{// PlayMontage
		PlayRotationMontage(Angle);
	}

	AnimInstance->SetMovementScale(FindVectorToDirection(WayPoints[0]));

	return 10.f > FVector::Distance(GetActorLocation(), WayPoints.Last());
}
FVector2D AEA_MasterEnemy::FindVectorToDirection(const FVector& Location)
{
	FVector2D TargetVector2D = FVector2D (Location);
	FVector2D OwnerVector2D = FVector2D(GetActorLocation());

	FVector2D DirVector = TargetVector2D - OwnerVector2D;

	DirVector.Normalize(0.0001f);

	float ForwardScale = UKismetMathLibrary::DotProduct2D(DirVector,FVector2D(GetActorForwardVector()));
	float RightScale = UKismetMathLibrary::DotProduct2D(DirVector, FVector2D(GetActorRightVector()));

	ForwardScale = UKismetMathLibrary::Acos(ForwardScale);
	RightScale = UKismetMathLibrary::Acos(RightScale);

	ForwardScale = UKismetMathLibrary::RadiansToDegrees(ForwardScale);
	RightScale = UKismetMathLibrary::RadiansToDegrees(RightScale);
	
	ForwardScale =ForwardScale / 180;
	RightScale	 =RightScale / 180;

	ForwardScale = UKismetMathLibrary::Lerp(1.f, -1.f, ForwardScale);
	RightScale = UKismetMathLibrary::Lerp(1.f, -1.f, RightScale);

	return FVector2D(RightScale, ForwardScale);
}
float AEA_MasterEnemy::FindAngle(const FVector TargetLocation)
{
	FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

	FVector TargetForward = UKismetMathLibrary::GetForwardVector(LookAtRotator);
	FVector OwnerForward = this->GetActorForwardVector();

	float TargetAngle = UKismetMathLibrary::DegAtan2(TargetForward.X, TargetForward.Y);
	float OwnerAngle = UKismetMathLibrary::DegAtan2(OwnerForward.X, OwnerForward.Y);

	return OwnerAngle - TargetAngle;
}
void AEA_MasterEnemy::PlayRotationMontage(float Angle)
{
	if (Angle >= 0.f)
	{
		if (UKismetMathLibrary::Abs(Angle) > 170.f)PlayAnimMontage(AM_TrunR180);
		else PlayAnimMontage(AM_TrunR90);
	}
	else
	{
		if (UKismetMathLibrary::Abs(Angle) > 170.f)PlayAnimMontage(AM_TrunL180);
		else PlayAnimMontage(AM_TrunL90);
	}
}
void AEA_MasterEnemy::SetSprint(bool spint)
{
	AnimInstance->SetSprint(spint);
}
void AEA_MasterEnemy::SprintCheck()
{
	if (AnimInstance->GetCombatMode())
	{
		if (EnemyController->GetBB_TargetActor())
		{
			FVector TargetLocation2 = EnemyController->GetBB_TargetActor()->GetActorLocation();
			float TargetDistance = FVector2D::Distance(FVector2D(TargetLocation2), FVector2D(GetActorLocation()));

			(TargetDistance > 300.f) ? SetSprint(true) : SetSprint(false);
		}
	}
	else
	{
		FVector TargetLocation2 = WayPoints.Last();
		float TargetDistance = FVector2D::Distance(FVector2D(TargetLocation2), FVector2D(GetActorLocation()));
		(TargetDistance > 300.f) ? SetSprint(true) : SetSprint(false);
	}
}
#pragma endregion
#pragma region Interface_CombatInteraction
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

	SprintCheck();

	if (AnimInstance->GetCombatMode()) return PlayCombatMove();
	else
	{
		float lookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), WayPoints[0]).Yaw;
		this->SetActorRotation(FRotator(0.f,lookAt, 0.f));
		AnimInstance->SetMovementScale(FVector2D(0.f,1.f));
	}
	return 10.f > FVector::Distance(GetActorLocation(), WayPoints.Last());
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