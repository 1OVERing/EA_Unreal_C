#include "NS_RootmotionAutoMoveY.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFrameWork/Character.h"

void UNS_RootmotionAutoMoveY::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	auto Owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Owner)return;

	float Axis = MeshComp->GetAnimInstance()->GetCurveValue(TEXT("CurveAxisY"));
	Axis = UKismetMathLibrary::Abs(Axis);
	if (BackMove)Axis = Axis * -1.f;

	FVector Distance = UKismetMathLibrary::GetForwardVector(Owner->GetControlRotation()) * Axis;
	Distance *= MeshComp->GetWorld()->DeltaTimeSeconds;
	Distance *= 10.f;
	Distance *= MoveScale;

	Owner->SetActorLocation(Owner->GetActorLocation() + Distance, true);
}