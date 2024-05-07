// Copyright Epic Games, Inc. All Rights Reserved.
#pragma region Include
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Project_ZSCharacter.h"

#include "Curves/CurveVector.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#pragma endregion
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AProject_ZSCharacter

AProject_ZSCharacter::AProject_ZSCharacter():
#pragma region Initialize
#pragma region Enum
	MovementState(EMovementState::EMS_NONE),
	PrevMovementState(EMovementState::EMS_NONE),
	MovementAction(EMovementAction::EMA_NONE),
	RotationMode(ERotationMode::ERM_LookingDirection),
	Gait(EGait::EGT_Walking),
	Stance(EStance::EMA_Standing),
	ViewMode(EViewMode::EVM_ThirdPerson),
	OverlayState(EOverlayState::EOS_Default),

	DesiredRotationMode(ERotationMode::ERM_LookingDirection),
	DesiredGait(EGait::EGT_Running),
	DesiredStance(EStance::EMA_Standing),
#pragma endregion
#pragma region Essential Values
	Acceleration(0.f),
	bIsMoving(false),
	bHasMovementInput(false),
	LastVelocityRotation(0.f),
	LastMovementInputRotation(0.f),
	Speed(0.f),
	MovementInputAmount(0.f),
	AimYawRate(0.f),
#pragma endregion
#pragma region Rotation Systems
	TargetRotation(0.f)

#pragma endregion

#pragma endregion

{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AProject_ZSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	Initialize_ZSValues();
}

void AProject_ZSCharacter::Initialize_ZSValues()
{
	/*Get movement data from the Movement Model Data table and set the Movement Data Struct.
	This allows you to easily switch out movement behaviors.*/

	const FString MovementTablePath = TEXT("DataTable'/Game/__Game_Files/Data/DataTables/MovementModelTable.MovementModelTable'");
	UDataTable* MovementTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *MovementTablePath));

	//"/Script/Engine.DataTable'/Game/__Game_Files/Data/DataTables/MovementModelTable.MovementModelTable'"

	if (MovementTableObject)
	{
		switch (DesiredGait)
		{
		case EGait::EGT_Walking:
			MovementData = MovementTableObject->FindRow<FMovementSettings_State>(FName("Normal"), TEXT(""));
			break;
		case EGait::EGT_Running:
			MovementData = MovementTableObject->FindRow<FMovementSettings_State>(FName("Sluggish"), TEXT(""));
			break;
		case EGait::EGT_Sprinting:
			MovementData = MovementTableObject->FindRow<FMovementSettings_State>(FName("Responsive"), TEXT(""));
			break;
		default:
			break;
		}
		//MovementData = MovementTableObject->FindRow<FMovementSettings_State>(FName("Normal"), TEXT(""));
	}


	OnGaitChanged(DesiredGait);
	OnRotationModeChanged(DesiredRotationMode);

}

void AProject_ZSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetEssentialValues();
	CacheValues();
	DrawDebugShapes();

	switch (MovementState)
	{
	case EMovementState::EMS_NONE:
		break;
	case EMovementState::EMS_Grounded:
		UpdateCharacterMovement();
		UpdateGroundedRotation();
		break;
	case EMovementState::EMS_InAir:
		break;
	case EMovementState::EMS_Mantling:
		break;
	case EMovementState::EMS_Ragdoll:
		break;
	default:
		break;
	}
}

void AProject_ZSCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	switch (GetCharacterMovement()->MovementMode)
	{

	case EMovementMode::MOVE_Walking:
		//MovementState = EMovementState::EMS_Grounded;
		SetMovementState_Implementation(EMovementState::EMS_Grounded);
		break;

	case EMovementMode::MOVE_Falling:

		//MovementState = EMovementState::EMS_InAir;
		SetMovementState_Implementation(EMovementState::EMS_InAir);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProject_ZSCharacter::SetGait_Implementation(EGait NewGait)
{
	if (NewGait == Gait) return;
	OnGaitChanged(NewGait);
	
}

void AProject_ZSCharacter::SetMovementState_Implementation(EMovementState NewMovementState)
{
	if (NewMovementState != MovementState)
	{
		OnMovementStateChanged(NewMovementState);
	}
	switch (MovementState)
	{
	case EMovementState::EMS_NONE:
		break;
	case EMovementState::EMS_Grounded:
		break;
	case EMovementState::EMS_InAir:
		if (Stance == EStance::EMA_Crouching)
		{
			UnCrouch();
		}
		break;
	case EMovementState::EMS_Mantling:
		break;
	case EMovementState::EMS_Ragdoll:
		break;
	default:
		break;
	}
}

void AProject_ZSCharacter::OnMovementStateChanged(EMovementState NewMovementState)
{
	EMovementState PreviousMovementState;
	SetPreviousAndNewValue(NewMovementState, MovementState, PreviousMovementState);
}

void AProject_ZSCharacter::OnGaitChanged(EGait NewActualGait)
{
	if (NewActualGait == Gait) return;
	
	EGait PreiviousGait;
	SetPreviousAndNewValue(NewActualGait, Gait, PreiviousGait);
}

void AProject_ZSCharacter::OnRotationModeChanged(ERotationMode NewRotationMode)
{
	ERotationMode PreviousRotation;
	SetPreviousAndNewValue(NewRotationMode, RotationMode, PreviousRotation);
}

void AProject_ZSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProject_ZSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProject_ZSCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AProject_ZSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AProject_ZSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

#pragma region Do Every Frame
void AProject_ZSCharacter::SetEssentialValues()
{
	// Set the amount of Acceleration
	Acceleration = CalculateAcceleration();

	// Calculate Character Speed from Velocity X and Y.
	FVector TempVe = GetVelocity();
	TempVe.Z = 0.f;
	Speed = TempVe.Size();

	// Make sure that character is Moving
	bIsMoving = Speed > 1.0f ? true : false;

	// If the Character Moving then take the Character Rotation from Velocity
	if (bIsMoving)
	{
		LastVelocityRotation = GetVelocity().Rotation();
	}

	// Determine Character Movement Input
	MovementInputAmount = GetCharacterMovement()->GetCurrentAcceleration().Size() / GetCharacterMovement()->GetMaxAcceleration();

	// Make sure the character movement button pressed 
	bHasMovementInput = MovementInputAmount > 0.f ? true : false;

	// If Character is moving get the rotation of the movement
	if (bHasMovementInput)
	{
		LastMovementInputRotation = GetCharacterMovement()->GetCurrentAcceleration().Rotation();
	}
	


	// Set the Speed of camera Rotation by Dividing differenc of Control rotation Yaw and previous AimYaw with deltaseconds
	AimYawRate = abs((GetControlRotation().Yaw - PreviousAimYaw) / GetWorld()->GetDeltaSeconds());


}

void AProject_ZSCharacter::CacheValues()
{
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = GetControlRotation().Yaw;
}

void AProject_ZSCharacter::DrawDebugShapes()
{
	DrawVelocityArrow();
	DrawMovementInputArror();
	DrawTargetRotationArrow();
	DrawAimingCone();
	DrawCapsule();

}

void AProject_ZSCharacter::DrawVelocityArrow()
{
	FVector Start = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	bool Velo = GetVelocity().Equals(FVector(0.f), 0.0001);
	FVector EndVec = Velo ? LastVelocityRotation.Vector() : GetVelocity();
	float EndLength = UKismetMathLibrary::MapRangeClamped(GetVelocity().Size(),
		0.0f,
		GetCharacterMovement()->MaxWalkSpeed,
		50.f,75.f);

	FVector End = Start + (EndVec.GetUnsafeNormal() * EndLength);
	FColor Color = Velo ? FColor::Yellow : FColor::Orange;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),
		Start,
		End,
		60.f,
		Color,
		0.0f,
		5.f);

}

void AProject_ZSCharacter::DrawMovementInputArror()
{
	FVector Start = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()-3.5f);
	bool Velo = GetCharacterMovement()->GetCurrentAcceleration().Equals(FVector(0.f), 0.0001);
	FVector EndVec = Velo ? LastMovementInputRotation.Vector() : GetCharacterMovement()->GetCurrentAcceleration();
	float RnageValue = GetCharacterMovement()->GetCurrentAcceleration().Size() / GetCharacterMovement()->GetMaxAcceleration();
	float EndLength = UKismetMathLibrary::MapRangeClamped(RnageValue,
		0.0f,
		1.f,
		50.f, 75.f);

	FVector End = Start + (EndVec.GetUnsafeNormal() * EndLength);
	FColor Color = Velo ? FColor::Cyan : FColor::Purple;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),
		Start,
		End,
		50.f,
		Color,
		0.0f,
		3.f);
}

void AProject_ZSCharacter::DrawTargetRotationArrow()
{
	FVector Start = GetActorLocation() - FVector(0.f,0.f,GetCapsuleComponent()->GetScaledCapsuleHalfHeight()-7.f);
	//FVector End = TargetRotation.Vector() * 50.f;
}

void AProject_ZSCharacter::DrawAimingCone()
{
	FVector Origin = GetMesh()->GetSocketLocation("FP_Head");
	FVector Direction = GetControlRotation().Vector().GetUnsafeNormal();

	UKismetSystemLibrary::DrawDebugCone(GetWorld(), Origin, Direction,
		100.f,
		30.f,
		30.f,
		8.f,
		FColor::Blue,
		0.f, 5.f);
}

void AProject_ZSCharacter::DrawCapsule()
{
	UKismetSystemLibrary::DrawDebugCapsule(GetWorld(),
		GetActorLocation(),
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),GetCapsuleComponent()->GetScaledCapsuleRadius(),
		GetActorRotation(),
		FColor::Black,
		0.f,
		0.3f);
}

FVector AProject_ZSCharacter::CalculateAcceleration()
{
	return (GetVelocity() - PreviousVelocity) / GetWorld()->GetDeltaSeconds();
}

#pragma endregion

#pragma region Do While On Ground
void AProject_ZSCharacter::UpdateCharacterMovement()
{
	// Get the movement type based on the current player state
	EGait AllowedGait = GetAllowedGait();
	EGait ActualGait = GetActualGait(AllowedGait);
	if (ActualGait != Gait)
	{
		SetGait_Implementation(ActualGait);
	}
	// Make the MovementSetting based on the allowed Gait
	UpdateDynamicMovementSttings(AllowedGait);
}
void AProject_ZSCharacter::UpdateGroundedRotation()
{
	if (CanUpdateMovingRotation())
	{
		FRotator  TargRot = FRotator(0.f, LastVelocityRotation.Yaw, 0.f);
		if (RotationMode == ERotationMode::ERM_LookingDirection)
		{
			if (Gait == EGait::EGT_Walking || Gait == EGait::EGT_Running)
			{
				FRotator ActorRot = FRotator(0.f, GetControlRotation().Yaw + GetAnimCurveValue("YawOffset"), 0.f);
				SmoothChracterRotation(ActorRot, 500.f, CalculateGroundRotationRate());
			}
			else
			{
				SmoothChracterRotation(FRotator(0.f, LastVelocityRotation.Yaw, 0.f), 500.f, CalculateGroundRotationRate());

			}
		}
		else if (RotationMode == ERotationMode::ERM_Aiming)
		{
			SmoothChracterRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f), 1000, CalculateGroundRotationRate());
		}
	}
	else
	{
		if (RotationMode == ERotationMode::ERM_LookingDirection)
		{
			RotationAmout();
		}
		else if (RotationMode == ERotationMode::ERM_Aiming)
		{
			LimitRotation(-100.f, 100.f, 20.f);
			RotationAmout();
		}
	}

}
EGait AProject_ZSCharacter::GetAllowedGait()
{
	switch (Stance)
	{
	case EStance::EMA_Standing:
		switch (RotationMode)
		{
		case ERotationMode::ERM_VelocityDirection:
			break;
		case ERotationMode::ERM_LookingDirection:
			switch (DesiredGait)
			{
			case EGait::EGT_Walking:
				return EGait::EGT_Walking;
			case EGait::EGT_Running:
				return EGait::EGT_Running;
			case EGait::EGT_Sprinting:
				if (CanSprint())
				{
					return EGait::EGT_Sprinting;
				}
				else
				{
					return EGait::EGT_Running;
				}
			default:
				break;
			}
			break;
		case ERotationMode::ERM_Aiming:
			switch (DesiredGait)
			{
			case EGait::EGT_Walking:
				return EGait::EGT_Walking;
			case EGait::EGT_Running:
				return EGait::EGT_Running;
			case EGait::EGT_Sprinting:
				return EGait::EGT_Running;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
	case EStance::EMA_Crouching:
		switch (DesiredGait)
		{
		case EGait::EGT_Walking:
			return EGait::EGT_Walking;
		case EGait::EGT_Running:
			return EGait::EGT_Running;
		case EGait::EGT_Sprinting:
			return EGait::EGT_Running;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return EGait();
}
EGait AProject_ZSCharacter::GetActualGait(EGait m_AllowedGait)
{
	float LocalWalkSpeed = CurrentMovementSettings.WalkSpeed;
	float LocalRunSpeed = CurrentMovementSettings.RunSpeed;
	float LocalSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed >= LocalRunSpeed + 10.f)
	{
		switch (m_AllowedGait)
		{
		case EGait::EGT_Walking:
			return EGait::EGT_Running;
		case EGait::EGT_Running:
			return EGait::EGT_Running;
		case EGait::EGT_Sprinting:
			return EGait::EGT_Sprinting;
		default:
			break;

		}
	}
	else if (Speed >= LocalWalkSpeed + 10.f)
		return EGait::EGT_Running;
	else
		return EGait::EGT_Running;


	return EGait();
}
bool AProject_ZSCharacter::CanSprint()
{
	if (!bHasMovementInput) return false;
	if (RotationMode == ERotationMode::ERM_Aiming) return false;
	if (RotationMode == ERotationMode::ERM_LookingDirection)
		return MovementInputAmount > 0.9f && abs(UKismetMathLibrary::NormalizedDeltaRotator(GetCharacterMovement()->GetCurrentAcceleration().Rotation(),GetControlRotation()).Yaw) < 50.f;
	return false;
}
void AProject_ZSCharacter::UpdateDynamicMovementSttings(EGait m_AllowedGait)
{
	// Get the Current MovementSettings based on the m_AllowedGait
	CurrentMovementSettings = GetTargetMovementSettings();
	// Update the speed based on the character m_AllowedGait
	switch (m_AllowedGait)
	{
	case EGait::EGT_Walking:
		GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSettings.WalkSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CurrentMovementSettings.WalkSpeed;
		break;
	case EGait::EGT_Running:
		GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSettings.RunSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CurrentMovementSettings.RunSpeed;
		break;
	case EGait::EGT_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSettings.SprintSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CurrentMovementSettings.SprintSpeed;
		break;
	default:
		break;
	}

	// Adjust the Acceleration, Deceleraion and Ground friction based on the Movement Curve
	if (CurrentMovementSettings.MovementCurve)
	{
		FVector CurveValue = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed());
		
		GetCharacterMovement()->MaxAcceleration = CurveValue.X;
		GetCharacterMovement()->BrakingDecelerationWalking = CurveValue.Y;
		GetCharacterMovement()->GroundFriction = CurveValue.Z;
		
	}
}
FMovementSettings AProject_ZSCharacter::GetTargetMovementSettings()
{
	switch (RotationMode)
	{
	case ERotationMode::ERM_VelocityDirection:
		break;
	case ERotationMode::ERM_LookingDirection:
		switch (Stance)
		{
		case EStance::EMA_Standing:
			return MovementData->LookingDirection.Standing;
		case EStance::EMA_Crouching:
			return MovementData->LookingDirection.Crouching;
		default:
			break;
		}
		break;
	case ERotationMode::ERM_Aiming:
		switch (Stance)
		{
		case EStance::EMA_Standing:
			return MovementData->Aiming.Standing;
		case EStance::EMA_Crouching:
			return MovementData->Aiming.Crouching;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return FMovementSettings();
}
float AProject_ZSCharacter::GetMappedSpeed()
{
	float LocalWalkSpeed = CurrentMovementSettings.WalkSpeed;
	float LocalRunSpeed = CurrentMovementSettings.RunSpeed;
	float LocalSprintSpeed = CurrentMovementSettings.SprintSpeed;

	float MappedWalkSpeed = UKismetMathLibrary::MapRangeClamped(Speed, 0.0f, LocalWalkSpeed, 0.0f, 1.f);
	float MappedRunSpeed = UKismetMathLibrary::MapRangeClamped(Speed, LocalWalkSpeed, LocalRunSpeed, 1.0f, 2.f);
	float MappedSprintSpeed = UKismetMathLibrary::MapRangeClamped(Speed, LocalRunSpeed, LocalSprintSpeed, 2.0f, 3.f);

	if (Speed > LocalWalkSpeed)
	{
		return MappedRunSpeed;
	}
	else if (Speed > LocalRunSpeed)
	{
		return MappedSprintSpeed;
	}
	else
	{
		return MappedWalkSpeed;
	}
	return 0.0f;
}
bool AProject_ZSCharacter::CanUpdateMovingRotation()
{
	return ((bIsMoving && bHasMovementInput) || Speed > 150.f) && !HasAnyRootMotion();
}
void AProject_ZSCharacter::SmoothChracterRotation(FRotator Target, float TargetInterp, float ActorInterpSpeed)
{
	TargetRotation = UKismetMathLibrary::RInterpTo_Constant(TargetRotation, Target, GetWorld()->GetDeltaSeconds(), TargetInterp);
	FRotator NewRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), ActorInterpSpeed);
	SetActorRotation(NewRot);
}
float AProject_ZSCharacter::CalculateGroundRotationRate()
{
	if (CurrentMovementSettings.RotationCurve)
	{
		float RotationRateCurve = CurrentMovementSettings.RotationCurve->GetFloatValue(GetMappedSpeed());
		return RotationRateCurve * UKismetMathLibrary::MapRangeClamped(AimYawRate, 0.0f, 300.f, 1.f, 3.f);
	}
	return 0.0f;
}
float AProject_ZSCharacter::GetAnimCurveValue(FName CurveName)
{
	if (GetMesh()->GetAnimInstance())
		return GetMesh()->GetAnimInstance()->GetCurveValue(CurveName);
	return 0.0f;
}
void AProject_ZSCharacter::LimitRotation(float AnimYawMin, float AnimYawMax, float InterpSpeed)
{
	FRotator m_RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	if (UKismetMathLibrary::InRange_FloatFloat(m_RotationDelta.Yaw,AnimYawMin, AnimYawMax))
	{
		float AimRot = m_RotationDelta.Yaw > 0.f ? GetControlRotation().Yaw + AnimYawMin : GetControlRotation().Yaw + AnimYawMax;
		SmoothChracterRotation(FRotator(0.f, AimRot, 0.f), 0.f, InterpSpeed);
	}
}
void AProject_ZSCharacter::RotationAmout()
{
	if (abs(GetAnimCurveValue("RotationAmount") > 0.001))
	{
		float ActorRotationYaw = GetAnimCurveValue("RotationAmount") * (GetWorld()->GetDeltaSeconds() / (1.f / 30.f));
		AddActorWorldRotation(FRotator(0.f, ActorRotationYaw, 0.f));
		TargetRotation = GetActorRotation();
	}
}
#pragma endregion

#pragma region Do While In Air
#pragma endregion


