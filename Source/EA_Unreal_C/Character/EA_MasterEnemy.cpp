#include "EA_MasterEnemy.h"
#include "../Controller/AIC_MasterEnemy.h"
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
	this->AIControllerClass = AAIC_MasterEnemy::StaticClass();
}

void AEA_MasterEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEA_MasterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region Interface_CombatInteraction
void AEA_MasterEnemy::PlayKnockBack_Implementation(){}
void AEA_MasterEnemy::PlayStiffen_Implementation(){}
bool AEA_MasterEnemy::PlayCatchAttack_Implementation(UAnimMontage* montage, FName sectionName){return false;}
#pragma endregion
#pragma region Interface_AIMove
bool AEA_MasterEnemy::CustomMoveStart_Implementation(){return false;}
bool AEA_MasterEnemy::CustomMoveTo_Implementation(){return false;}
bool AEA_MasterEnemy::CustomMoveEnd_Implementation(){return false;}
#pragma endregion