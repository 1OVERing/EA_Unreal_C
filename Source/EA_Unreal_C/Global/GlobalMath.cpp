#include "GlobalMath.h"
#include "kismet/KismetMathLibrary.h"

FVector2D CustomMath::FindVectorToDirection(const AActor* OwnerActor, const FVector& TargetLocation)
{
	FVector2D TargetVector2D = FVector2D(TargetLocation);
	FVector2D OwnerVector2D = FVector2D(OwnerActor->GetActorLocation());

	FVector2D DirVector = TargetVector2D - OwnerVector2D;

	DirVector.Normalize(0.0001f);

	float ForwardScale = UKismetMathLibrary::DotProduct2D(DirVector, FVector2D(OwnerActor->GetActorForwardVector()));
	float RightScale = UKismetMathLibrary::DotProduct2D(DirVector, FVector2D(OwnerActor->GetActorRightVector()));

	ForwardScale = UKismetMathLibrary::Acos(ForwardScale);
	RightScale = UKismetMathLibrary::Acos(RightScale);

	ForwardScale = UKismetMathLibrary::RadiansToDegrees(ForwardScale);
	RightScale = UKismetMathLibrary::RadiansToDegrees(RightScale);

	ForwardScale = ForwardScale / 180;
	RightScale = RightScale / 180;

	ForwardScale = UKismetMathLibrary::Lerp(1.f, -1.f, ForwardScale);
	RightScale = UKismetMathLibrary::Lerp(1.f, -1.f, RightScale);

	return FVector2D(RightScale, ForwardScale);
}

float CustomMath::GetTargetAngle(const AActor* OwnerActor, const FVector& TargetLocation)
{
	FVector2D Dir = CustomMath::FindVectorToDirection(OwnerActor, TargetLocation);
	Dir.Y = Dir.Y - 1.f;
	Dir.Y = UKismetMathLibrary::Abs(Dir.Y);
	Dir.Y *= 90.f;
	return Dir.Y;
}
