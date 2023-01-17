#include "EA_MasterAnimInstance.h"
#include "GameFramework/Character.h"

UEA_MasterAnimInstance::UEA_MasterAnimInstance()
{
}

void UEA_MasterAnimInstance::NativeBeginPlay()
{
}
void UEA_MasterAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	if (true == IsSprint && MovementScale.Y >= 1.f && MovementScale.X == 0.f) SprintTime += deltaSeconds;
	else SprintTime = 0.f;
}

void UEA_MasterAnimInstance::SetMovementScale(const FVector2D& scale)
{
	PrevMovementScale = MovementScale;
	MovementScale = scale;
}

bool UEA_MasterAnimInstance::PlayJumping(FVector LaunchDirection)
{
	if (GetCurrentActiveMontage() != nullptr) return false;

	Jumping = true;
	JumpDirection = LaunchDirection;
	return true;
}

void UEA_MasterAnimInstance::AnimNotify_Jumping()
{
	auto Owner = Cast<ACharacter>(TryGetPawnOwner());
	if (!Owner->IsValidLowLevel()) return;
	Owner->Jump();
	
	FTimerHandle LaunchTimer;
	auto Lambda = FTimerDelegate::CreateLambda([&]()
	{
		auto Owner = Cast<ACharacter>(TryGetPawnOwner());
		Owner->LaunchCharacter(JumpDirection, false, false);
	});

	GetWorld()->GetTimerManager().SetTimer(LaunchTimer, Lambda, 0.1f, false);
}
void UEA_MasterAnimInstance::LandedEvent(const FHitResult& Hit)
{
	Jumping = false;
}

void UEA_MasterAnimInstance::SetCombatMode(bool mode)
{
	CombatMode = mode;
}