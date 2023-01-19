#include "EA_MasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EA_MasterAnimInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../InputSystem/IMC_Movement.h"
#include "../InputSystem/IMC_Combat.h"

AEA_MasterCharacter::AEA_MasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
#pragma region MeshSetting
	/* Face */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Face(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Separates/Heads/SK_Head_Cyber.SK_Head_Cyber"));
	if (Face.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Face.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
	/* TopBodies */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TopBodies(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Separates/TopBodies/SK_TopBody_K.SK_TopBody_K"));
	if (Face.Succeeded())
	{
		USkeletalMeshComponent* SK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_TopBodies"));
		SK->SetSkeletalMesh(TopBodies.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK->AttachToComponent(GetMesh(), Rules);
		SK->SetLeaderPoseComponent(GetMesh());
	}
	/* BotBodies */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BotBodies(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Separates/BotBodies/SK_BotBody_B.SK_BotBody_B"));
	if (BotBodies.Succeeded())
	{
		USkeletalMeshComponent* SK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_BotBodies"));
		SK->SetSkeletalMesh(BotBodies.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK->AttachToComponent(GetMesh(), Rules);
		SK->SetLeaderPoseComponent(GetMesh());
	}
	/* Hand_L */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Hand_L(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Separates/Hands/SK_HandCyber_L.SK_HandCyber_L"));
	if (Hand_L.Succeeded())
	{
		USkeletalMeshComponent* SK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_Hand_L"));
		SK->SetSkeletalMesh(Hand_L.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK->AttachToComponent(GetMesh(), Rules);
		SK->SetLeaderPoseComponent(GetMesh());
	}
	/* Hand_R */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Hand_R(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Separates/Hands/SK_HandCyber_R.SK_HandCyber_R"));
	if (Hand_R.Succeeded())
	{
		USkeletalMeshComponent* SK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_Hand_R"));
		SK->SetSkeletalMesh(Hand_R.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK->AttachToComponent(GetMesh(), Rules);
		SK->SetLeaderPoseComponent(GetMesh());
	}
	/* BotBodies */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Feet(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Separates/Shoes/SK_Feet.SK_Feet"));
	if (Feet.Succeeded())
	{
		USkeletalMeshComponent* SK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_Feet"));
		SK->SetSkeletalMesh(Feet.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK->AttachToComponent(GetMesh(), Rules);
		SK->SetLeaderPoseComponent(GetMesh());
	}

	/* Helmet */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Helmet(TEXT("/Game/Characters/PP801_P3/Meshes/Characters/Separates/Hoods/SK_HelmetGlass.SK_HelmetGlass"));
	if (Helmet.Succeeded())
	{
		USkeletalMeshComponent* SK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_Helmet"));
		SK->SetSkeletalMesh(Helmet.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SK->AttachToComponent(GetMesh(), Rules, FName("HAIR"));
	}
	/* Helmet */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Twinblades(TEXT("/Game/Meshes/Weapon/Twinblades/Twinblades_Common_Sprint.Twinblades_Common_Sprint"));
	if (Twinblades.Succeeded())
	{
		UStaticMeshComponent* SM_l = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SK_Blades_l"));
		UStaticMeshComponent* SM_r = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SK_Blades_r"));
		SM_l->SetStaticMesh(Twinblades.Object);
		SM_r->SetStaticMesh(Twinblades.Object);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		SM_l->AttachToComponent(GetMesh(), Rules, FName("ik_hand_l"));
		SM_r->AttachToComponent(GetMesh(), Rules, FName("ik_hand_r"));
	}
#pragma endregion
#pragma region EnhancedInputSystem
	if (!IMC_Movement) IMC_Movement = CreateDefaultSubobject<UIMC_Movement>(TEXT("IMC_Movement"));
	if (!IMC_Combat) IMC_Combat = CreateDefaultSubobject<UIMC_Combat>(TEXT("IMC_Combat"));
#pragma endregion
#pragma region Animation
	/* Animation */
	ConstructorHelpers::FClassFinder<UEA_MasterAnimInstance> Instance(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/AnimationBlueprints/ABP_Master.ABP_Master_C'"));
	if (Instance.Succeeded())
	{
		GetMesh()->SetAnimClass(Instance.Class);
	}
#pragma endregion
#pragma region CamaraSetting

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(GetRootComponent());
	CameraArm->TargetArmLength = 200.0f;
	CameraArm->SetRelativeLocation(FVector(0.f, 0.f, 45.f));
	CameraArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
#pragma endregion
#pragma region MovementSetting

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.0f, 0.0f);
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
#pragma endregion
}
void AEA_MasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* playercontroller = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playercontroller->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(IMC_Movement, 0);
			subsystem->AddMappingContext(IMC_Combat, 0);
		}
	}
	{
		auto Anim = Cast<UEA_MasterAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)AnimInstance = Anim; /* 여기 델리게이트 문제임 */
		AnimInstance->OnMontageEnded.AddDynamic(this,&AEA_MasterCharacter::EndedMontage);
	}
	{
		LandedDelegate.AddDynamic(this, &AEA_MasterCharacter::LandedEvent);
	}
}
void AEA_MasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AEA_MasterCharacter::CharacterSetter(FName CharacterName, UAnimMontage* EquipMontage, UAnimMontage* DodgeMontage)
{
	AM_Equip = EquipMontage;
	AM_Dodge = DodgeMontage;
}
#pragma region InputSystemFunc
void AEA_MasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!IMC_Movement || !IMC_Combat) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		/* IMC_Movement */
		{
			/* Movement */
			EnhancedInputComponent->BindAction(IMC_Movement->GetMovementAction(), ETriggerEvent::Completed, this, &AEA_MasterCharacter::MoveAction_Completed);
			EnhancedInputComponent->BindAction(IMC_Movement->GetMovementAction(), ETriggerEvent::Triggered, this, &AEA_MasterCharacter::MoveAction_Triggered);
			/* Look */
			EnhancedInputComponent->BindAction(IMC_Movement->GetLookAction(), ETriggerEvent::Triggered, this, &AEA_MasterCharacter::LookAction);
			EnhancedInputComponent->BindAction(IMC_Movement->GetLookAction(), ETriggerEvent::Completed, this, &AEA_MasterCharacter::LookAction);
			/* Sprint */
			EnhancedInputComponent->BindAction(IMC_Movement->GetSprintAction(), ETriggerEvent::Started, this, &AEA_MasterCharacter::SprintAction);
			EnhancedInputComponent->BindAction(IMC_Movement->GetSprintAction(), ETriggerEvent::Completed, this, &AEA_MasterCharacter::SprintAction);
			/* Jump */
			EnhancedInputComponent->BindAction(IMC_Movement->GetJumpAction(), ETriggerEvent::Started, this, &AEA_MasterCharacter::JumpAction);
		}

		/* Combat */
		{
			/* Equip */
			EnhancedInputComponent->BindAction(IMC_Combat->GetEquipAction(), ETriggerEvent::Started, this, &AEA_MasterCharacter::EquipAction);
			/* LMouse */
			EnhancedInputComponent->BindAction(IMC_Combat->GetLMouseAction(), ETriggerEvent::Started, this, &AEA_MasterCharacter::LMouseAction);
			/* RMouse */
			EnhancedInputComponent->BindAction(IMC_Combat->GetRMouseAction(), ETriggerEvent::Started, this, &AEA_MasterCharacter::RMouseAction);
		}
	}
}

void AEA_MasterCharacter::MoveAction_Triggered(const FInputActionValue& Value)
{
	if (!IsEquip && !IsJumping)
	{
		FVector2D InputDirction = Value.Get<FVector2D>();
		FVector ControlForwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		FVector ControlRightVector = UKismetMathLibrary::GetRightVector(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		ControlForwardVector *= InputDirction.Y;
		ControlRightVector *= InputDirction.X;
		FRotator NewActorRotation = FVector(ControlForwardVector + ControlRightVector).Rotation();
		SetActorRotation(NewActorRotation);

		(IsSprint) ? InputDirction = FVector2D(0.f, 2.f) : InputDirction = FVector2D(0.f, 1.f);
		MovementScale = InputDirction;
		AnimInstance->SetMovementScale(InputDirction);
	}
	if (IsEquip && !IsJumping)
	{
		SetActorRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		FVector2D InputDirction = Value.Get<FVector2D>();
		MovementScale = InputDirction;
		AnimInstance->SetMovementScale(InputDirction);
	}
}
void AEA_MasterCharacter::MoveAction_Completed(const FInputActionValue& Value)
{
	IsSprint = false;
	AnimInstance->SetSprint(false);
	MovementScale = Value.Get<FVector2D>();
	AnimInstance->SetMovementScale(FVector2D::ZeroVector);
}
void AEA_MasterCharacter::LookAction(const FInputActionValue& Value)
{
	FVector2D inputScale = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(inputScale.X);
		AddControllerPitchInput(inputScale.Y);
	}
}
void AEA_MasterCharacter::SprintAction(const FInputActionValue& Value)
{
	AnimInstance->SetSprint(true);
	IsSprint = true;
}
void AEA_MasterCharacter::JumpAction(const FInputActionValue& Value)
{
	IsJumping = AnimInstance->PlayJumping(FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.f));
}
void AEA_MasterCharacter::EquipAction(const FInputActionValue& Value)
{
	if (GetCurrentMontage() == nullptr && !GetCharacterMovement()->IsFalling())
	{
		IsEquip = !IsEquip;
		if (IsEquip) PlayAnimMontage(AM_Equip, 1.f, FName("Equip"));
		else PlayAnimMontage(AM_Equip, 1.f, FName("UnEquip"));

		FString message;
		IsEquip ? message = FString("Equip") : message = FString("NotEquip");
		GEngine->AddOnScreenDebugMessage(3, 1.f, FColor(1, 1, 1), message);
		AnimInstance->SetCombatMode(IsEquip);
	}
}
void AEA_MasterCharacter::LMouseAction(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(1011, 1.f, FColor::Green, TEXT("LMouse"));

	PlayAnimMontage(NormalAttackCombo);
}
void AEA_MasterCharacter::RMouseAction(const FInputActionValue& Value)
{
	if (!IsEquip) return;
	FName SectionName = "";
	if (GetCurrentMontage() != AM_Dodge)
	{
		if (MovementScale.Y > 0.f) SectionName = TEXT("Slide_F");
		else if (MovementScale.Y < 0.f) SectionName = TEXT("Slide_B");
		else if (MovementScale.X > 0.f) SectionName = TEXT("Slide_R");
		else if (MovementScale.X < 0.f) SectionName = TEXT("Slide_L");
		else SectionName = TEXT("Slide_B");
	}
	else
	{
		if(-1 != UKismetStringLibrary::FindSubstring(AnimInstance->Montage_GetCurrentSection(AM_Dodge).ToString(), "Dodge")) return;

		if (MovementScale.Y > 0.f) SectionName = TEXT("Dodge_F");
		else if (MovementScale.Y < 0.f) SectionName = TEXT("Dodge_B");
		else if (MovementScale.X > 0.f) SectionName = TEXT("Dodge_R");
		else if (MovementScale.X < 0.f) SectionName = TEXT("Dodge_L");
		else SectionName = TEXT("Dodge_B");
	}

	PlayAnimMontage(AM_Dodge,1.f,SectionName);
}
#pragma endregion

#pragma region Movement
void AEA_MasterCharacter::LandedEvent(const FHitResult& Hit)
{
	AnimInstance->LandedEvent(Hit);
	IsJumping = false;
}
#pragma endregion
#pragma region Animation
void AEA_MasterCharacter::EndedMontage(UAnimMontage* Montage, bool bInterrupted)
{
	GEngine->AddOnScreenDebugMessage(10112, 1.f, FColor::Green, Montage->GetFName().ToString());
}
#pragma endregion