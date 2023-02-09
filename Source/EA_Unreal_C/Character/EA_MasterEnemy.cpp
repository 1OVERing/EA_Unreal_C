#include "EA_MasterEnemy.h"
#include "../Controller/AIC_MasterEnemy.h"
#include "../Character/EA_MasterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "../Global/GlobalCombat.h"
#include "../Global/GlobalMath.h"
#define RotationAllowableRange 5.f
#define DistanceAllowableRange 0.f
#define RotationSpeed 100.f
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
	AnimInstance->OnMontageBlendingOut.AddDynamic(this, &AEA_MasterEnemy::MontageBledOut);
	EnemyController = Cast<AAIC_MasterEnemy>(GetController());
	EnemyController->OnSightTarget.AddDynamic(this, &AEA_MasterEnemy::SightTarget);
	if (!AnimInstance || !EnemyController)
	{
		GEngine->AddOnScreenDebugMessage(999, 10.f, FColor::Red, TEXT("Failed Create AnimInstance & Controller (AEA_MasterEnemy::BeginPlay)"));
		this->Destroy();
	}
}
void AEA_MasterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AEA_MasterEnemy::MontageBledOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AM_Equip)
	{
		WeaponEquip(AnimInstance->GetCombatMode());
	}
}

#pragma region Combat
void AEA_MasterEnemy::SetMontages_Hit(UAnimMontage* Forward, UAnimMontage* Backward, UAnimMontage* Right, UAnimMontage* Left)
{
	AM_Hit_Forward = Forward;
	AM_Hit_Backward = Backward;
	AM_Hit_Right = Right;
	AM_Hit_Left = Left;
}
void AEA_MasterEnemy::SetMontages_Attacks(UAnimMontage* Equip, TArray<struct FSkillSet> Skills)
{
	AM_Equip = Equip;
	SkillSet.Empty();
	SkillSet = Skills;
}
float AEA_MasterEnemy::GetTargetDistance()
{
	if (!EnemyController->GetBB_TargetActor()) return -1.f;

	ACharacter* Target = Cast<ACharacter>(EnemyController->GetBB_TargetActor());

	float Distance = FVector2D::Distance(FVector2D(GetActorLocation()), FVector2D(Target->GetActorLocation()));
	float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius() + Target->GetCapsuleComponent()->GetScaledCapsuleRadius();
	Distance -= Radius;

	return Distance;
}
float AEA_MasterEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CanHit())
	{
		if (DamageCauser) PlayHitAnimMontage(DamageCauser);



	}
	return 1.f;
}
void AEA_MasterEnemy::WeaponEquip(bool equip)
{
	if (equip)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK_Weapon->AttachToComponent(GetMesh(), Rules, FName("ik_hand_gun"));
	}
	else
	{
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK_Weapon->AttachToComponent(GetMesh(), Rules, FName("KatanaSheath"));
	}
}
void AEA_MasterEnemy::PlayHitAnimMontage(const AActor* Causer)
{
	{// 예외처리
	}

	StopAnimMontage();
	FVector2D Dir = CustomMath::FindVectorToDirection(this, Causer->GetActorLocation());
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
	SetHitTimer(HitTime);
}
void AEA_MasterEnemy::SightTarget_Implementation(AActor* SightTarget)
{
	if (SightTarget)
	{
		if (!AnimInstance->GetCombatMode())
		{
			SetHitTimer(PlayAnimMontage(AM_Equip, 1.f, TEXT("Equip")));
			WeaponEquip(true);
			AnimInstance->SetCombatMode(true);
		}
		SetNextAttack();
	}
	else
	{
		SetHitTimer(PlayAnimMontage(AM_Equip, 1.f, TEXT("UnEquip")));
		AnimInstance->SetCombatMode(false);
	}
}
void AEA_MasterEnemy::HitEndTimerFunc_Implementation()
{
	GetWorldTimerManager().ClearTimer(HitTimer);
}
void AEA_MasterEnemy::SetHitTimer(float Time)
{
	GetWorldTimerManager().ClearTimer(HitTimer);
	GetWorldTimerManager().SetTimer(HitTimer, this, &AEA_MasterEnemy::HitEndTimerFunc, Time, false);
}
bool AEA_MasterEnemy::IsHit()
{
	return HitTimer.IsValid();
}
bool AEA_MasterEnemy::CanHit()
{
	return true;
}
bool AEA_MasterEnemy::IsHitReaction()
{
	return HitTimer.IsValid();
}
#pragma endregion
#pragma region Movement
void AEA_MasterEnemy::SetMontages_Trun(UAnimMontage* am_L180, UAnimMontage* am_R180)
{
	AM_TrunL180 = am_L180;
	AM_TrunR180 = am_R180;
}
bool AEA_MasterEnemy::FindWaypoint(const FVector MoveToLocation)
{
	WayPoints.Empty();
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
	return (montage == AM_TrunL180 || montage == AM_TrunR180);
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
float AEA_MasterEnemy::FindAngle(const FVector TargetLocation)
{
	FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

	FVector TargetForward = UKismetMathLibrary::GetForwardVector(LookAtRotator);
	FVector OwnerForward = this->GetActorForwardVector();

	float TargetAngle = UKismetMathLibrary::DegAtan2(TargetForward.X, TargetForward.Y);
	float OwnerAngle = UKismetMathLibrary::DegAtan2(OwnerForward.X, OwnerForward.Y);

	return OwnerAngle - TargetAngle;
}
void AEA_MasterEnemy::PlayRotationMontage(FVector2D Angle)
{
	if (Angle.X >= 0.f)
	{
		PlayAnimMontage(AM_TrunR180);
	}
	else
	{
		PlayAnimMontage(AM_TrunL180);
	}
}
bool AEA_MasterEnemy::RotationCheck(const FVector& TargetLocation)
{
	if (::IsValid(EnemyController->GetBB_TargetActor()))
	{
		float Angle = ::CustomMath::GetTargetAngle(this, EnemyController->GetBB_TargetActor()->GetActorLocation());

		if (Angle <= RotationAllowableRange) return true;
		else return false;
	}
	else
	{
		float Angle = ::CustomMath::GetTargetAngle(this, TargetLocation);
		if (Angle <= RotationAllowableRange) return true;
		else return false;
	}
}
bool AEA_MasterEnemy::DistanceCheck(const FVector& TargetLocation)
{
	bool zeroMove = WayPoints.IsEmpty();
	if (zeroMove)
	{
		AnimInstance->SetMovementScale(FVector2D::ZeroVector);
		return true;
	}
	float Distance = FVector2D::Distance(FVector2D(this->GetActorLocation()), FVector2D(TargetLocation));
	return (Distance <= DistanceAllowableRange);
}
void AEA_MasterEnemy::SetRotation(const FVector& TargetLocation)
{
	FVector2D Dir;

	if (AnimInstance->GetCombatMode() && ::IsValid(EnemyController->GetBB_TargetActor()))
	{
		Dir = CustomMath::FindVectorToDirection(this, EnemyController->GetBB_TargetActor()->GetActorLocation());
		Dir.Y = Dir.Y - 1.f;
		Dir.Y = UKismetMathLibrary::Abs(Dir.Y);
		Dir.Y *= 90.f;

		if (Dir.Y <= 45.f && !InRotation())
		{
			FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyController->GetBB_TargetActor()->GetActorLocation());
			float speed = RotationSpeed * GetWorld()->GetDeltaSeconds();
			float RotationDir = LookAtRotator.Yaw - GetActorRotation().Yaw;

			if (RotationDir >= 0)
			{
				this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw + speed, 0.f));
			}
			else
			{
				this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw - speed, 0.f));
			}
		}
		else if(!InRotation()) PlayRotationMontage(Dir);
		else
		{
			if (Dir.Y <= 10.f)
			{
				StopAnimMontage();
			}
		}
		/*if (Dir.Y >= 150.f) PlayRotationMontage(Dir);
		else
		{
			FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyController->GetBB_TargetActor()->GetActorLocation());
			float speed = RotationSpeed * GetWorld()->GetDeltaSeconds();
			float RotationDir = LookAtRotator.Yaw - GetActorRotation().Yaw;

			if (RotationDir >= 0)
			{
				this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw + speed, 0.f));
			}
			else
			{
				this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw - speed, 0.f));
			}
		}*/
	}
	else if (!AnimInstance->GetCombatMode() && !WayPoints.IsEmpty())
	{
		float LookAtRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), WayPoints[0]).Yaw;
		
		SetActorRotation(FRotator(GetActorRotation().Pitch, LookAtRotator, GetActorRotation().Roll));
		/*Dir = CustomMath::FindVectorToDirection(this, WayPoints[0]);
		Dir.Y = Dir.Y - 1.f;
		Dir.Y = UKismetMathLibrary::Abs(Dir.Y);
		Dir.Y *= 90.f;
		
		FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), WayPoints[0]);
		float speed = RotationSpeed * GetWorld()->GetDeltaSeconds();
		float RotationDir = LookAtRotator.Yaw - GetActorRotation().Yaw;
		bool Forward = CustomMath::FindVectorToDirection(this, WayPoints[0]).Y >= 0;
		GEngine->AddOnScreenDebugMessage(12312, 1.f, FColor::Red, UKismetStringLibrary::Conv_FloatToString(Forward));
		if (RotationDir >= 0)
		{
			if(Forward) this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw + speed, 0.f));
			else this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw - speed, 0.f));
			
		}
		else
		{
			if (Forward) this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw - speed, 0.f));
			else this->SetActorRotation(FRotator(0.f, GetActorRotation().Yaw + speed, 0.f));
		}*/
	}
}
void AEA_MasterEnemy::SetMove(const FVector& TargetLocation)
{
	if (!WayPoints.IsEmpty() && GetNextMovePoint() != FVector::ZeroVector)
	{
		FVector2D MoveScale = CustomMath::FindVectorToDirection(this, GetNextMovePoint());
		AnimInstance->SetMovementScale(MoveScale);
	}
	else AnimInstance->SetMovementScale(FVector2D::ZeroVector);
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
	else if (!WayPoints.IsEmpty())
	{
		FVector TargetLocation2 = WayPoints.Last();
		float TargetDistance = FVector2D::Distance(FVector2D(TargetLocation2), FVector2D(GetActorLocation()));
		(TargetDistance > 300.f) ? SetSprint(true) : SetSprint(false);
	}
	else SetSprint(false);
}
#pragma endregion

#pragma region Interface_CombatInteraction
void AEA_MasterEnemy::PlayKnockBack_Implementation() {}
void AEA_MasterEnemy::PlayStiffen_Implementation() {}
bool AEA_MasterEnemy::PlayCatchAttack_Implementation(UAnimMontage* montage, FName sectionName)
{
	if (!CanHit()) return false;
	StopAnimMontage();
	float HitTime = 0.f;
	PlayAnimMontage(montage, 1.f, sectionName);
	HitTime = montage->GetSectionLength(montage->GetSectionIndex(sectionName));
	SetHitTimer(HitTime);
	return true;
}

void AEA_MasterEnemy::SetNextAttack_Implementation()
{
	if (EnemyController->GetBB_TargetActor())
	{
		float TargetDistance = GetTargetDistance();
		int BestSkillIndex = -1;
		float BestDistance = -1;

		TArray<int> indexs;
		TArray<int> negativeIndexs;

		// 거리에 따른 즉발 가능 스킬과 아닌 스킬을 구분
		for (int i = 0; i < SkillSet.Num(); ++i)
		{
			float Range = TargetDistance - SkillSet[i].AllowableRange;
			(Range >= 0) ? indexs.Emplace(i) : negativeIndexs.Emplace(i);
		}

		// 구분된 스킬중 즉발스킬을 우선적으로 랜덤으로 착출하여 세팅
		if (!indexs.IsEmpty())
		{// 즉발 가능
			indexs.Emplace(-1);
			int randomIndex = UKismetMathLibrary::RandomInteger(indexs.Num());

			// 이동 후 로직 강제 실행
			if (indexs[randomIndex] == -1 && !negativeIndexs.IsEmpty()) CurrentSkillIndex = UKismetMathLibrary::RandomInteger(negativeIndexs.Num());

			else CurrentSkillIndex = randomIndex;
		}
		// 이동 후 공격
		else CurrentSkillIndex = UKismetMathLibrary::RandomInteger(negativeIndexs.Num());

	}
	else CurrentSkillIndex = UKismetMathLibrary::RandomInteger(SkillSet.Num());
	CurrentSkillIndex = UKismetMathLibrary::Clamp(CurrentSkillIndex, 0, SkillSet.Num() - 1);

	EnemyController->SetBB_AllowableRange(SkillSet[CurrentSkillIndex].AllowableRange);
}
float AEA_MasterEnemy::PlayAttack_Implementation()
{
	return PlayAnimMontage(SkillSet[CurrentSkillIndex].montage);
}
bool AEA_MasterEnemy::AttackEndCheck_Implementation()
{
	return false;
}
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
	if (IsHit()) return true;
	if (!WayPoints.IsEmpty() && MoveToLocation != WayPoints.Last())
	{
		CustomMoveStart(MoveToLocation);
	}
	// 1. 회전 체크
	// 2. 무브 체크
	// 3. 전투용 무브와 노말무브 따로 설정
	// 4. 회전이 안끝났는데 무브가 끝나면 회전 강제 회전

	// 1. 회전체크
	bool RotCheck = RotationCheck(MoveToLocation);
	if (!RotCheck) SetRotation(MoveToLocation);
	// 2. 무브체크
	bool DisCheck = DistanceCheck(MoveToLocation);
	if (!DisCheck) SetMove(MoveToLocation);

	SprintCheck();

	return RotCheck && DisCheck && !InRotation();
	/*FVector NextWaypoint = GetNextMovePoint();

	if (NextWaypoint == FVector::ZeroVector && RotationCheck()) return true;



	if (AnimInstance->GetCombatMode()) return PlayCombatMove();
	else
	{
		float lookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), WayPoints[0]).Yaw;
		this->SetActorRotation(FRotator(0.f, lookAt, 0.f));
		AnimInstance->SetMovementScale(FVector2D(0.f, 1.f));
	}
	return 1.f >= FVector::Distance(GetActorLocation(), WayPoints.Last());*/
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