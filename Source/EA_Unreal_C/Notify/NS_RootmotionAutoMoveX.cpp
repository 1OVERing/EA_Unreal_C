#include "NS_RootmotionAutoMoveX.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFrameWork/Character.h"


void UNS_RootmotionAutoMoveX::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	auto Owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Owner)return;

	float Axis = MeshComp->GetAnimInstance()->GetCurveValue(TEXT("CurveAxisX"));
	Axis = UKismetMathLibrary::Abs(Axis);
	if (LeftMove)Axis = Axis * -1.f;

	FVector Distance = UKismetMathLibrary::GetRightVector(Owner->GetControlRotation()) * Axis;
	Distance *= MeshComp->GetWorld()->DeltaTimeSeconds;
	Distance *= 10.f;
	Distance *= MoveScale;

	Owner->SetActorLocation(Owner->GetActorLocation() + Distance,true);
}