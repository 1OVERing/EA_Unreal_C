// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_CollisionOff.h"
#include "GameFrameWork/Character.h"
#include "Components/CapsuleComponent.h"

void UNS_CollisionOff::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ACharacter* Owner = Cast<ACharacter>(MeshComp->GetOwner());
	if(::IsValid(Owner)) Owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UNS_CollisionOff::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACharacter* Owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (::IsValid(Owner)) Owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
