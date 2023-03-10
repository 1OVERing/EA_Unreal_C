#include "EA_MasterEnemy.h"
#include "../Controller/AIC_MasterEnemy.h"
#include "../Character/EA_MasterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "../Global/GlobalMath.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "../UI/CharacterStatusHUD.h"


#define RotationAllowableRange 5.f
#define DistanceAllowableRange 0.f
#define RotationSpeed 100.f
#define GuardTime 1.f

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
	CharacterStat.ResetCharacterStat();

	HUD_CharacterStatusComp = CreateDefaultSubobject<UWidgetComponent>("UI_CharacterStatus");
	HUD_CharacterStatusComp->SetupAttachment(GetRootComponent(), "UI_CharacterStatus");
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
	//HUD
	HUD_CharacterStatus = Cast<UCharacterStatusHUD>(HUD_CharacterStatusComp->GetWidget());
	if (!::IsValid(HUD_CharacterStatus)) HUD_CharacterStatus = nullptr;
	else
	{
		HUD_CharacterStatusComp->SetWidgetSpace(EWidgetSpace::Screen);
		HUD_CharacterStatusComp->SetTickMode(ETickMode::Automatic);
		HUD_CharacterStatusComp->SetDrawSize(FVector2D(100.f, 20.f));
		HUD_CharacterStatus->OwnerActor = this;
		UpdateHUDCharacterStat();
	}
}
void AEA_MasterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsGuard() && CharacterStat.CurStamina != CharacterStat.MaxStamina)
	{
		CharacterStat.CurStamina = UKismetMathLibrary::FClamp(CharacterStat.CurStamina + (DeltaTime * GuardHealingSpeed), 0.f, CharacterStat.MaxStamina);
	}
	else if(IsGuard()  && ::IsValid(EnemyController->GetBB_TargetActor()))
	{
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyController->GetBB_TargetActor()->GetActorLocation());
		SetActorRotation(FRotator(GetActorRotation().Pitch, Rot.Yaw, GetActorRotation().Roll));
	}
}
void AEA_MasterEnemy::MontageBledOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AM_Equip)
	{
		WeaponEquip(AnimInstance->GetCombatMode());
	}
}
bool AEA_MasterEnemy::GetCurrentMontageSectionCheck(int count, ...)
{
	if (count <= 0) return false;

	va_list list;
	va_start(list, count);

	for (int i = 0; i < count; ++i) if (AnimInstance->Montage_GetCurrentSection() == va_arg(list, FName))
	{
		va_end(list);
		return true;
	}

	va_end(list);
	return false;
}
void AEA_MasterEnemy::UpdateHUDCharacterStat()
{
	if (HUD_CharacterStatus)
	{
		HUD_CharacterStatus->SetHealth(CharacterStat.CurHP, CharacterStat.MaxHP);
		HUD_CharacterStatus->SetStamina(CharacterStat.CurStamina, CharacterStat.MaxStamina);
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
void AEA_MasterEnemy::SetMontages_Attacks(UAnimMontage* Equip, UAnimMontage* Guard, TArray<struct FSkillSet> Skills)
{
	AM_Equip = Equip;
	SkillSet.Empty();
	SkillSet = Skills;
	AM_Guard = Guard;
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
	if (IsGuard())
	{
		if (this->CharacterGuardTakeDamage(Damage + 4))
		{
			AnimInstance->Montage_JumpToSection("Hit");
			return 0.f;
		}
		else
		{
			AnimInstance->Montage_JumpToSection("Broken");
			return 0;
		}
	}
	this->CharacterTakeDamage(Damage);
	if (CanHit() && EventInstigator && DamageCauser)
	{
		if (DamageCauser) PlayHitAnimMontage(DamageCauser);

		SightTarget(DamageCauser);
		EnemyController->SetBB_TargetActor(DamageCauser);
	}
	else if (!EventInstigator && DamageCauser)
	{// ?????? ??????
		CharacterStat.TakeDamage(Damage);
	}
	return Damage;
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
	{// ????????
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
		this->SetNextAttack();
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
void AEA_MasterEnemy::ResetSkillRecharge(int SkillIndex)
{
	SkillSet[SkillIndex].RechargeTimeFinish = true;
}
bool AEA_MasterEnemy::IsHit()
{
	return HitTimer.IsValid();
}
bool AEA_MasterEnemy::CanHit()
{
	return true;
}
void AEA_MasterEnemy::CharacterTakeDamage(float Damage)
{
	if (CharacterStat.TakeDamage(Damage) <= 0)
	{// ????
		UnPossessed();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->GetCharacterMovement()->GravityScale = 0.f;
		AnimInstance->SetDead(true);
		EnemyController->SetLogicEnable(false);
	}

	UpdateHUDCharacterStat();
}
bool AEA_MasterEnemy::CharacterGuardTakeDamage(float Damage)
{
	bool result = false;
	CharacterStat.TakeStaminaPoint(Damage);
	(CharacterStat.CurStamina > 0) ? result = true : result = false;

	UpdateHUDCharacterStat();
	return result;
}
void AEA_MasterEnemy::PlayGuard()
{
	if (IsGuard()) return;

	GetWorldTimerManager().ClearTimer(GuardTimer);
	PlayAnimMontage(AM_Guard);
	EnemyController->SetLogicEnable(false);
	GetWorldTimerManager().SetTimer(GuardTimer, this, &AEA_MasterEnemy::EndGuard, GuardTime, false);
}
bool AEA_MasterEnemy::IsGuard()
{
	if (GuardTimer.IsValid())
	{
		if (AM_Guard == GetCurrentMontage()) return true;
		else
		{
			EndGuard();
			return false;
		}
	}
	else
	{
		if (AM_Guard != GetCurrentMontage()) return false;
		else
		{
			EndGuard();
			StopAnimMontage();
			return false;
		}
	}
	return false;
}
void AEA_MasterEnemy::EndGuard()
{
	GetWorldTimerManager().ClearTimer(GuardTimer);
	if (GetCurrentMontage() == AM_Guard && !GetCurrentMontageSectionCheck(2, FName("End"), FName("Broken")))
	{
		AnimInstance->Montage_JumpToSection("End");
	}
	EnemyController->SetLogicEnable(true);
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

	while (10.f >= TargetDistance)
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
		else if (!InRotation()) PlayRotationMontage(Dir);
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

bool AEA_MasterEnemy::SetNextAttack_Implementation()
{
	if (IsHit() || !EnemyController) return false;
	if (!EnemyController->GetBB_TargetActor()) return false;
	float TargetDistance = GetTargetDistance();

	TArray<int> indexs;
	TArray<int> negativeIndexs;
	CurrentSkillIndex = -1;

	// ?????? ???? ???? ???? ?????? ???? ?????? ????
	for (int i = 0; i < SkillSet.Num(); ++i)
	{
		float Range = TargetDistance - SkillSet[i].AllowableMinRange;
		if (SkillSet[i].RechargeTimeFinish) (Range >= 0) ? negativeIndexs.Emplace(i) : indexs.Emplace(i);
	}
	if (indexs.IsEmpty() && negativeIndexs.IsEmpty()) return false;

	// ?????? ?????? ?????????? ?????????? ???????? ???????? ????
	if (!indexs.IsEmpty())
	{// ???? ????
		int randomIndex = UKismetMathLibrary::RandomInteger(indexs.Num());
		// ???? ?? ???? ???? ????
		if (UKismetMathLibrary::RandomBool() && !negativeIndexs.IsEmpty())CurrentSkillIndex = negativeIndexs[UKismetMathLibrary::RandomInteger(negativeIndexs.Num())];
		else CurrentSkillIndex = indexs[randomIndex];
	}
	// ???? ?? ????
	else if (!negativeIndexs.IsEmpty())
	{
		CurrentSkillIndex = negativeIndexs[UKismetMathLibrary::RandomInteger(negativeIndexs.Num())];
	}
	else return false;

	EnemyController->SetBB_AllowableMaxRange(SkillSet[CurrentSkillIndex].AllowableMaxRange);
	EnemyController->SetBB_AllowableMinRange(SkillSet[CurrentSkillIndex].AllowableMinRange);

	float TargetDis = GetTargetDistance();
	if (TargetDis != -1 && TargetDis <= SkillSet[CurrentSkillIndex].AllowableMaxRange && TargetDis >= SkillSet[CurrentSkillIndex].AllowableMinRange)
	{
		float LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyController->GetBB_TargetActor()->GetActorLocation()).Yaw;
		SetActorRotation(FRotator(GetActorRotation().Pitch, LookAt, GetActorRotation().Roll));
		EnemyController->SetBB_DirectAttackEnable(true);
	}
	else EnemyController->SetBB_DirectAttackEnable(false);

	return true;
}
float AEA_MasterEnemy::PlayAttack_Implementation()
{
	if (CurrentSkillIndex == -1) return 0.f;
	AnimInstance->SetMovementScale(FVector2D::ZeroVector);
	if (UKismetMathLibrary::RandomInteger(3) == 0) // ???? ???????? 30%
	{
		PlayGuard();
		return -1;
	}

	if (SkillSet[CurrentSkillIndex].RechargeTime != 0.f)
	{
		FTimerHandle LaunchTimer;
		auto Lambda = FTimerDelegate::CreateUFunction(this, "ResetSkillRecharge", CurrentSkillIndex);

		GetWorld()->GetTimerManager().SetTimer(LaunchTimer, Lambda, SkillSet[CurrentSkillIndex].RechargeTime, false);
		SkillSet[CurrentSkillIndex].RechargeTimeFinish = false;
	}
	return PlayAnimMontage(SkillSet[CurrentSkillIndex].montage);
}
bool AEA_MasterEnemy::AttackEndCheck_Implementation()
{
	if (IsGuard()) return true;
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
	// 1. ???? ????
	// 2. ???? ????
	// 3. ?????? ?????? ???????? ???? ????
	// 4. ?????? ?????????? ?????? ?????? ???? ???? ????

	// 1. ????????
	bool RotCheck = RotationCheck(MoveToLocation);
	if (!RotCheck) SetRotation(MoveToLocation);
	// 2. ????????
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