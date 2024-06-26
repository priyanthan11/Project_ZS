// Fill out your copyright notice in the Description page of Project Settings.

#pragma region Include
#include "ZS_AnimInstance.h"
#include "Project_ZSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#pragma endregion

UZS_AnimInstance::UZS_AnimInstance() :
#pragma region Initialization
	Condition(true),
	PreviousCondtion(Condition),
	

#pragma region Character Information
	AAimingRotation(0.f),
	AVelocity(0.f),
	AAcceleration(0.f),
	bAHasMovementInput(false),
	bAIsMoving(false),
	ASpeed(0.f),
	AMovementInput(0.f),
	AAimYawRate(0.f),
	AI_MovementMode(EMovementMode::MOVE_None),
	AI_MovementAction(EMovementAction::EMA_LowMantle),
	AI_RotationMode(ERotationMode::ERM_VelocityDirection),
	AI_Gait(EGait::EGT_Running),
	AI_Stance(EStance::EMA_Standing),
	AI_ViewMode(EViewMode::EVM_ThirdPerson),
	AI_OverlayState(EOverlayState::EOS_Default),
#pragma endregion
#pragma region AnimGraph Grounded
	AI_GroundEntryState(EGroundEntryState::EGE_None),
	MovementDirection(EMovementDirection::EMD_Forward),
	TrackedHipDirection(EHipDirection::EHD_F),
	RelativeAccelerationAmount(0.f),
	RotateL(false),
	RotateR(false),
	Pivot(false),
	RotateRate(1.f),
	RotationScale(0.f),
	DiagonalScaleAmount(0.f),
	WalkRunBlend(0.f),
	StandingPlayRate(1.f),
	CrouchingPlayRate(1.f),
	StrideBlend(0.f),
	FYaw(0.f),
	BYaw(0.f),
	LYaw(0.f),
	RYaw(0.f),
#pragma endregion
#pragma region AnimGraph InAir
	JumpPlayRate(1.2f),
	FallSpeed(0.f),
	LandPrediction(1.f),
#pragma endregion
#pragma region AnimGraph Aiming Values
	SmoothedAimingRotation(0.f),
	SpineRotation(0.f),
	AimingAngle(0.f),
	SmoothedAimingAngle(0.f),
	AimSweepTime(0.5f),
	InputYawOffsetTime(0.f),
	ForwardYawTime(0.f),
	LeftYawTime(0.f),
	RightYawTime(0.f),
#pragma endregion
#pragma region AnimGraph LayerBlending
	EnableAimOffset(1.f),
	BasePoseN(1.f),
	BasePoseCLF(0.f),
	ArmLAdd(0.f),
	ArmLLS(0.f),
	ArmLMS(0.f),
	ArmRAdd(0.f),
	ArmRLS(0.f),
	ArmRMS(0.f),
	HandL(0.f),
	HandR(0.f),
	SpineAdd(0.f),
	HeadAdd(0.f),
	EnableHandIKL(1.f),
	EnableHandIKR(1.f),

#pragma endregion
#pragma region AnimGraph FootIK
FootLockLAlpha(0.f),
FootLockLLocation(0.f),
FootLockLRotation(0.f),
FootLockRAlpha(0.f),
FootLockRLocation(0.f),
FootLockRRotation(0.f),
FootOffsetLLocation(0.f),
FootOffsetLRotation(0.f),
FootOffsetRLocation(0.f),
FootOffsetRRotation(0.f),
PelvisAlpha(0.f),
#pragma endregion
#pragma region AnimGraph Turn In Place
	ElapsedDelayTime(0.f),

#pragma endregion
#pragma region Config
	FootHeight(13.5)
#pragma endregion
#pragma endregion
{
}

void UZS_AnimInstance::NativeInitializeAnimation()
{
	PlayerCharacter = Cast<AProject_ZSCharacter>(TryGetPawnOwner());
	PlayerInterface = Cast<IZS_Interface>(PlayerCharacter);
}

void UZS_AnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (PlayerCharacter == nullptr)
		PlayerCharacter = Cast<AProject_ZSCharacter>(TryGetPawnOwner());
	if (PlayerInterface == nullptr)
		PlayerInterface = Cast<IZS_Interface>(PlayerCharacter);

	DeltaX = DeltaTime;
	if (DeltaX != 0.0f)
	{
		if (PlayerCharacter)
		{
			UpdateCharacterInfor();
			UpdateAimingValues();
			UpdateLayerValues();
			UpdateFootIK();
			switch (AI_MovementState)
			{
			case EMovementState::EMS_NONE:
				break;
			case EMovementState::EMS_Grounded:
				DoWhileLoop();
				break;
			case EMovementState::EMS_InAir:
				UpdateInAirValues();
				break;
			case EMovementState::EMS_Mantling:
				break;
			case EMovementState::EMS_Ragdoll:
				break;
			default:
				break;
			}
		}
	}



}

void UZS_AnimInstance::UpdateCharacterInfor()
{
	if (PlayerCharacter->Implements<UZS_Interface>())
	{
		if (PlayerInterface)
		{
			PlayerInterface->Execute_GetEssentialValues(PlayerCharacter,
				AVelocity,AAcceleration,AMovementInput,bAIsMoving,bAHasMovementInput,ASpeed,
				AMovementInputAmount,AAimYawRate,AAimingRotation);
			PlayerInterface->Execute_GetCurrentStates(PlayerCharacter,
				AI_MovementMode, AI_MovementState, AI_PrevMovementState, AI_MovementAction, AI_RotationMode,
				AI_Gait, AI_Stance, AI_ViewMode, AI_OverlayState);
		}
	}
}

void UZS_AnimInstance::UpdateAimingValues()
{
	/*Interpolate the Aiming Rotation value to achieve smooth aiming rotation changes.
	Interpolating the rotation before calculating the angle ensures the value is not affected by changes in actor
	rotation, allowing slow aiming rotation changes with fast actor rotation changes.*/
	SmoothedAimingRotation = UKismetMathLibrary::RInterpTo(SmoothedAimingRotation, AAimingRotation, DeltaX, 10.f);


	/*Calculate the Aiming angle and Smoothed Aiming Angle by getting the delta between the aiming rotation and the
	actor rotation.*/
	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(AAimingRotation, PlayerCharacter->GetActorRotation());
	AimingAngle = { Delta.Yaw,Delta.Pitch };
	FRotator SDelta = UKismetMathLibrary::NormalizedDeltaRotator(SmoothedAimingRotation, PlayerCharacter->GetActorRotation());
	SmoothedAimingAngle = FVector2D(SDelta.Yaw, SDelta.Pitch);

	/*Clamp the Aiming Pitch Angle to a range of 1 to 0 for use in the vertical aim sweeps.*/

	//if (AI_RotationMode == ERotationMode::ERM_LookingDirection || AI_RotationMode == ERotationMode::ERM_Aiming)
	//{
	//	AimSweepTime = UKismetMathLibrary::MapRangeClamped(AimingAngle.Y, -90.f, 90.f, 1.f, 0.f);
	//	/*Use the Aiming Yaw Angle divided by the number of spine+pelvis bones to get the amount of spine rotation 
	//	needed to remain facing the camera direction.*/
	//	SpineRotation = FRotator(0.0f, AimingAngle.X / 4.f, 0.0f);
	//}

	switch (AI_RotationMode)
	{
	case ERotationMode::ERM_VelocityDirection:
		break;
	case ERotationMode::ERM_LookingDirection:
		AimSweepTime = UKismetMathLibrary::MapRangeClamped(AimingAngle.Y, -90.f, 90.f, 1.f, 0.f);
		/*Use the Aiming Yaw Angle divided by the number of spine+pelvis bones to get the amount of spine rotation
		needed to remain facing the camera direction.*/
		SpineRotation = FRotator(0.0f, AimingAngle.X / 4.f, 0.0f);
		break;
	case ERotationMode::ERM_Aiming:
		AimSweepTime = UKismetMathLibrary::MapRangeClamped(AimingAngle.Y, -90.f, 90.f, 1.f, 0.f);
		/*Use the Aiming Yaw Angle divided by the number of spine+pelvis bones to get the amount of spine rotation
		needed to remain facing the camera direction.*/
		SpineRotation = FRotator(0.0f, AimingAngle.X / 4.f, 0.0f);
		break;
	default:
		break;
	}



	/*Get the delta between the Movement Input rotation and Actor rotation and map it to a range of 0-1.
	This value is used in the aim offset behavior to make the character look toward the Movement Input.*/
	if (AI_RotationMode == ERotationMode::ERM_VelocityDirection)
	{
		if (bAHasMovementInput)
		{
			FRotator TempMovementInput = UKismetMathLibrary::MakeRotFromX(AMovementInput);
			FRotator TempDelta = UKismetMathLibrary::NormalizedDeltaRotator(TempMovementInput, PlayerCharacter->GetActorRotation());
			float TempDeltaYaw = UKismetMathLibrary::MapRangeClamped(TempDelta.Yaw, -180.f, 180.f, 0.0f, 1.0f);
			InputYawOffsetTime = UKismetMathLibrary::FInterpTo(InputYawOffsetTime, TempDeltaYaw, DeltaX, 8.f);

		}
	}

	/*Separate the Aiming Yaw Angle into 3 separate Yaw Times. These 3 values are used in the Aim Offset behavior
	to improve the blending of the aim offset when rotating completely around the character. This allows you to keep
	the aiming responsive but still smoothly blend from left to right or right to left.*/
	LeftYawTime = UKismetMathLibrary::MapRangeClamped(abs(SmoothedAimingAngle.X), 0.0f, 180.f, 0.5f, 0.0f);
	RightYawTime = UKismetMathLibrary::MapRangeClamped(abs(SmoothedAimingAngle.X), 0.0f, 180.f, 0.5f, 1.0f);
	ForwardYawTime = UKismetMathLibrary::MapRangeClamped(SmoothedAimingAngle.X, -180.f, 180.f, 0.0f, 1.0f);

}

void UZS_AnimInstance::UpdateLayerValues()
{
	/*Get the Aim Offset weight by getting the opposite of the Aim Offset Mask.*/
	EnableAimOffset = FMath::Lerp(1.0f, 0.0f, GetAnimCurve("Mask_AimOffset"));

	//Set the Base Pose weights
	BasePoseN = GetAnimCurve("BasePose_N");
	BasePoseCLF = GetAnimCurve("BasePose_CLF");

	//Set the Additive amount weights for each body part
	SpineAdd = GetAnimCurve("Layering_Spine_Add");
	HeadAdd = GetAnimCurve("Layering_Head_Add");
	ArmLAdd = GetAnimCurve("Layering_Arm_L_Add");
	ArmRAdd = GetAnimCurve("Layering_Arm_R_Add");

	//Set the Hand Override weights
	HandR = GetAnimCurve("Layering_Hand_R");
	HandL = GetAnimCurve("Layering_Hand_L");

	//Blend and set the Hand IK weights to ensure they only are weighted if allowed by the Arm layers.
	EnableHandIKL = FMath::Lerp(0.0f, GetAnimCurve("Enable_HandIK_L"), GetAnimCurve("Layering_Arm_L"));
	EnableHandIKR = FMath::Lerp(0.0f, GetAnimCurve("Enable_HandIK_R"), GetAnimCurve("Layering_Arm_R"));


	//Set whether the arms should blend in mesh space or local space. The Mesh space weight will always be 1 
	//unless the Local Space (LS) curve is fully weighted.
	ArmLLS = GetAnimCurve("Layering_Arm_L_LS");
	ArmRLS = GetAnimCurve("Layering_Arm_R_LS");
	ArmLMS = UKismetMathLibrary::FFloor(ArmLLS) - 1;
	ArmRMS = UKismetMathLibrary::FFloor(ArmRLS) - 1;
}

void UZS_AnimInstance::UpdateFootIK()
{
	SetFootLocking("Enable_FootIK_L", "FootLock_L", "ik_foot_l", FootLockLAlpha, FootLockLLocation, FootLockLRotation);
	SetFootLocking("Enable_FootIK_R", "FootLock_R", "ik_foot_r", FootLockRAlpha, FootLockRLocation, FootLockRRotation);
	if (AI_MovementState == EMovementState::EMS_NONE || AI_MovementState == EMovementState::EMS_Grounded || AI_MovementState == EMovementState::EMS_Mantling)
	{
		FVector FootOffsetLTarget, FootOffsetRTarget;
		SetFootOffset("Enable_FootIK_L","ik_foot_l","root",FootOffsetLTarget,FootOffsetLLocation,FootOffsetLRotation);
		SetFootOffset("Enable_FootIK_R", "ik_foot_r", "root", FootOffsetRTarget, FootOffsetRLocation, FootOffsetRRotation);
		SetPelvisIKOffset(FootOffsetLTarget, FootOffsetRTarget);
	}
	else if (AI_MovementState == EMovementState::EMS_InAir)
	{
		SetPelvisIKOffset(FVector(0.f), FVector(0.f));
		ResetIKOffsets();
	}
}

void UZS_AnimInstance::DoWhileMoving()
{
	UpdateMovementValues();
	UpdateRotationValues();
}

void UZS_AnimInstance::DoWhileNotMoving()
{
	RotateInPlaceCheck();
	TurnInPlaceCheck();
	if (CanDynamicallyTransition())
	{
		DynamicTranstionCheck();
	}
}

float UZS_AnimInstance::GetAnimCurve(FName Name)
{
	return GetCurveValue(Name);
}

void UZS_AnimInstance::DoWhileLoop()
{
	ShouldMove = ShouldMoveCheck();
	if (ShouldMove)
	{
		if (DoOnce.Execute())
		{
			DoOnce1.Reset();
			//ChangedToTrue();
			// Do When Start to Move
			ElapsedDelayTime = 0.f;
			RotateL = false;
			RotateR = false;
		}
		//WhileTrue();
		// Do While Moving
		DoWhileMoving();
	}
	else
	{
		if (DoOnce1.Execute())
		{
			DoOnce.Reset();
			//ChangedToFalse();
		}
		//WhileFalse();
		// Do While Not Moving
		DoWhileNotMoving();
	}

}

void UZS_AnimInstance::UpdateMovementValues()
{

	//Interp and set the Velocity Blend.
	VelocityBlend = InterpVelocityBlend(VelocityBlend, CalculateVelocityBlend(), 12.f, DeltaX);

	//Set the Diagnal Scale Amount.
	DiagonalScaleAmount = CalculateDiagonalScaleAmount();

	//Set the Relative Acceleration Amount and Interp the Lean Amount.
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();

	FLeanAmount tempAmount;
	tempAmount.LR = RelativeAccelerationAmount.Y;
	tempAmount.FB = RelativeAccelerationAmount.X;
	
	LeanAmount = InterpLeanAmount(LeanAmount, tempAmount, 4.f, DeltaX);;

	//Set the Walk Run Blend
	WalkRunBlend = CalculateWalkRunBlend();

	//Set the Stride Blend
	StrideBlend = CalculateStrideBlend();

	//Set the Standing and Crouching Play Rates
	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();
}

void UZS_AnimInstance::UpdateRotationValues()
{
	//Set the Movement Direction
	MovementDirection = CalculateMovementDirection();

	/*Set the Yaw Offsets. These values influence the "YawOffset" curve in the animgraph and are used to offset the
	characters rotation for more natural movement. The curves allow for fine control over how the offset behaves for
	each movement direction.*/

	float delta = UKismetMathLibrary::NormalizedDeltaRotator(AVelocity.Rotation(), PlayerCharacter->GetControlRotation()).Yaw;
	FYaw = YawOffsetFB->GetVectorValue(delta).X;
	BYaw = YawOffsetFB->GetVectorValue(delta).Y;
	LYaw = YawOffsetLR->GetVectorValue(delta).X;
	RYaw = YawOffsetLR->GetVectorValue(delta).Y;

}

FVelocityBlend UZS_AnimInstance::CalculateVelocityBlend()
{
	/*Calculate the Velocity Blend. This value represents the velocity amount of the actor in each direction
	(normalized so that diagonals equal 0.5 for each direction), and is used in a BlendMulti node to produce better
	directional blending than a standard blend space.*/
	//FVector NVector = AVelocity.GetSafeNormal(0.1f);

	//FVector LocRelativeVelocityDir = UKismetMathLibrary::Quat_UnrotateVector(PlayerCharacter->GetActorRotation().Quaternion(), AVelocity.GetSafeNormal(0.1f));
	
	FVector LocRelativeVelocityDir = PlayerCharacter->GetActorRotation().UnrotateVector(AVelocity.GetSafeNormal(0.1));


	/*float AbsX = abs(LocRelativeVelocityDir.X);
	float AbsY = abs(LocRelativeVelocityDir.Y);
	float AbsZ = abs(LocRelativeVelocityDir.Z);*/
	float Sum = abs(LocRelativeVelocityDir.X) + abs(LocRelativeVelocityDir.Y) + abs(LocRelativeVelocityDir.Z);

	// Normalize based on the sum
	FVector RelativeDirection = LocRelativeVelocityDir / Sum;

	FVelocityBlend temp;

	temp.F = UKismetMathLibrary::FClamp(RelativeDirection.X, 0.0f, 1.0f);
	temp.B = abs(UKismetMathLibrary::FClamp(RelativeDirection.X, -1.0f, 0.0f));
	temp.L = abs(UKismetMathLibrary::FClamp(RelativeDirection.Y, -1.0f, 0.0f));
	temp.R = UKismetMathLibrary::FClamp(RelativeDirection.Y, 0.0f, 1.0f);


	return temp;
}

FVelocityBlend UZS_AnimInstance::InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target, float InterpSpeed, float DeltaTime)
{
	FVelocityBlend temp;
	temp.F = UKismetMathLibrary::FInterpTo(Current.F, Target.F, InterpSpeed, DeltaTime);
	temp.B = UKismetMathLibrary::FInterpTo(Current.B, Target.B, InterpSpeed, DeltaTime);
	temp.L = UKismetMathLibrary::FInterpTo(Current.L, Target.L, InterpSpeed, DeltaTime);
	temp.R = UKismetMathLibrary::FInterpTo(Current.R, Target.R, InterpSpeed, DeltaTime);

	return temp;
}

FLeanAmount UZS_AnimInstance::InterpLeanAmount(FLeanAmount LA, FLeanAmount Target, float InterpSpeed, float Deltax)
{
	FLeanAmount temp;
	temp.LR = UKismetMathLibrary::FInterpTo(LA.LR, Target.LR, Deltax, InterpSpeed);
	temp.FB = UKismetMathLibrary::FInterpTo(LA.FB, Target.FB, Deltax, InterpSpeed);
	return temp;
}

bool UZS_AnimInstance::ShouldMoveCheck()
{
	return (bAIsMoving && bAHasMovementInput) || ASpeed > 150.f;
}

float UZS_AnimInstance::CalculateDiagonalScaleAmount()
{
	/*Calculate the Diagonal Scale Amount. This value is used to scale the Foot IK Root bone to make the Foot IK
	bones cover more distance on the diagonal blends. Without scaling, the feet would not move far enough on the
	diagonal direction due to the linear translational blending of the IK bones. The curve is used to easily map the
	value.*/

	return DiagonalScaleAmountCurve->GetFloatValue(abs(VelocityBlend.F + VelocityBlend.B));
}

float UZS_AnimInstance::CalculateStrideBlend()
{
	/*Calculate the Stride Blend. This value is used within the blendspaces to scale the stride (distance feet travel)
	so that the character can walk or run at different movement speeds. It also allows the walk or run gait animations
	to blend independently while still matching the animation speed to the movement speed, preventing the character
	from needing to play a half walk+half run blend. The curves are used to map the stride amount to the speed for
	maximum control.*/

	float temp1 = UKismetMathLibrary::Lerp(StrideBlendNWalk->GetFloatValue(ASpeed), StrideBlendNRun->GetFloatValue(ASpeed), GetAnimCuveClaped("Weight_Gait", -1.0f, 0.0f, 1.0f));
	return UKismetMathLibrary::Lerp(temp1, StrideBlendCWalk->GetFloatValue(ASpeed), GetCurveValue("BasePose_CLF"));
}

float UZS_AnimInstance::CalculateWalkRunBlend()
{
	/*Calculate the Walk Run Blend. This value is used within the Blendspaces to blend between walking and running.*/
	switch (AI_Gait)
	{
	case EGait::EGT_Walking:
		return 0.0f;
	case EGait::EGT_Running:
		return 1.0f;
	case EGait::EGT_Sprinting:
		return 1.0f;
	default:
		break;
	}

	return 0.0f;
}

float UZS_AnimInstance::CalculateStandingPlayRate()
{
	/*Calculate the Play Rate by dividing the Character's speed by the Animated Speed for each gait.
	The lerps are determined by the "Weight_Gait" anim curve that exists on every locomotion cycle so that the play
	rate is always in sync with the currently blended animation. The value is also divided by the Stride Blend and
	the mesh scale so that the play rate increases as the stride or scale gets smaller.*/

	float temp1 = UKismetMathLibrary::Lerp(ASpeed / 150.0f/*Animated WalkSpeed*/, ASpeed / 350.f/*AnimatedRunSpeed*/, GetAnimCuveClaped("Weight_Gait", -1.0f, 0.0f, 1.0f));
	float temp2 = UKismetMathLibrary::Lerp(temp1, ASpeed / 600.0f/*Animated SprintSpeed*/, GetAnimCuveClaped("Weight_Gait", -2.0f, 0.0f, 1.0f));

	return UKismetMathLibrary::FClamp((temp2 / StrideBlend) / GetOwningComponent()->GetComponentScale().Z, 0.0f, 3.0f);

}

float UZS_AnimInstance::CalculateCrouchingPlayRate()
{
	/*Calculate the Crouching Play Rate by dividing the Character's speed by the Animated Speed.
	This value needs to be separate from the standing play rate to improve the blend from crocuh to stand while in
	motion.*/

	return UKismetMathLibrary::FClamp(((ASpeed / 150.f/*Animated Crouch Speed*/) / StrideBlend) / GetOwningComponent()->GetComponentScale().Z, 0.0f, 2.0f);
}

FVector UZS_AnimInstance::CalculateRelativeAccelerationAmount()
{
	/*Calculate the Relative Acceleration Amount. This value represents the current amount of
	acceleration / deceleration relative to the actor rotation. It is normalized to a range of -1 to 1 so that -1
	equals the Max Braking Deceleration, and 1 equals the Max Acceleration of the Character Movement Component.*/
	if (UKismetMathLibrary::Dot_VectorVector(AAcceleration, AVelocity) > 0.0f)
	{
		FVector VecClMax = UKismetMathLibrary::Vector_ClampSizeMax(AAcceleration, PlayerCharacter->GetCharacterMovement()->GetMaxAcceleration());
		return UKismetMathLibrary::Quat_UnrotateVector(PlayerCharacter->GetActorRotation().Quaternion(), VecClMax / PlayerCharacter->GetCharacterMovement()->GetMaxAcceleration());

	}
	else
	{
		FVector VecClMax = UKismetMathLibrary::Vector_ClampSizeMax(AAcceleration, PlayerCharacter->GetCharacterMovement()->GetMaxBrakingDeceleration());
		return UKismetMathLibrary::Quat_UnrotateVector(PlayerCharacter->GetActorRotation().Quaternion(), VecClMax / PlayerCharacter->GetCharacterMovement()->GetMaxBrakingDeceleration());
	}
	return FVector();
}

float UZS_AnimInstance::GetAnimCuveClaped(FName Name, float Bias, float ClampMin, float ClampMax)
{
	return UKismetMathLibrary::FClamp(GetCurveValue(Name) + Bias, ClampMin, ClampMax);
}

EMovementDirection UZS_AnimInstance::CalculateMovementDirection()
{
	/*Calculate the Movement Direction. This value represents the direction the character is moving relative to the
	camera during the Looking Direction / Aiming rotation modes, and is used in the Cycle Blending Anim Layers to
	blend to the appropriate directional states.*/

	if (AI_Gait == EGait::EGT_Walking || AI_Gait == EGait::EGT_Running)
	{

		if (AI_RotationMode == ERotationMode::ERM_LookingDirection || AI_RotationMode == ERotationMode::ERM_Aiming)
		{
			return CalculateQuadrant(MovementDirection, 70.f, -70.f, 110.f, -110.f, 5.f, UKismetMathLibrary::NormalizedDeltaRotator(AVelocity.Rotation(), AAimingRotation).Yaw);
		}
		else
		{
			return EMovementDirection::EMD_Forward;
		}

	}
	else
	{
		return EMovementDirection::EMD_Forward;
	}


	//return EMovementDirection();
}

EMovementDirection UZS_AnimInstance::CalculateQuadrant(EMovementDirection MovementDir, float FR_Threshold, float FL_Threshold, float BR_Threshold, float BL_Threshold, float Buffer, float Angle)
{
	/*Take the input angle and determine its quadrant(direction).Use the current Movement Direction to increase or
		decrease the buffers on the angle ranges for each quadrant.*/
		if (AngleInRange(Angle, FL_Threshold, FR_Threshold, Buffer, MovementDir == EMovementDirection::EMD_Forward || MovementDir == EMovementDirection::EMD_Backward))
		{
			return EMovementDirection::EMD_Forward;
		}
		else if (AngleInRange(Angle, FR_Threshold, BR_Threshold, Buffer, MovementDir == EMovementDirection::EMD_Right || MovementDir == EMovementDirection::EMD_Left))
		{
			return EMovementDirection::EMD_Right;
		}
		else if (AngleInRange(Angle, BL_Threshold, FL_Threshold, Buffer, MovementDir == EMovementDirection::EMD_Right || MovementDir == EMovementDirection::EMD_Left))
		{
			return EMovementDirection::EMD_Left;
		}
		else
		{
			return EMovementDirection::EMD_Backward;
		}

	//return EMovementDirection();
}

bool UZS_AnimInstance::AngleInRange(float Angle, float MinAngle, float MaxAngle, float buffer, bool IncreaseBuffer)
{
	if (IncreaseBuffer)
	{
		return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle + buffer, MaxAngle - buffer, true, true);
	}
	else
	{
		return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle - buffer, MaxAngle + buffer, true, true);
	}


	//return false;
}

bool UZS_AnimInstance::CanRotateInPlace()
{
	//Only perform a Rotate In Place Check if the character is Aiming or in First Person.
	return AI_RotationMode == ERotationMode::ERM_Aiming || AI_ViewMode == EViewMode::EVM_FirstPerson;
}

void UZS_AnimInstance::RotateInPlaceCheck()
{
	if (CanRotateInPlace())
	{
		
		/*Step 1: Check if the character should rotate left or right by checking if 
		the Aiming Angle exceeds the threshold.*/
		RotateL = AimingAngle.X < -50.f /*Rotate Min Threshold*/;
		RotateR = AimingAngle.X > 50.f /* Rotate Max Threshold*/;

		/*Step 2: If the character should be rotating, set the Rotate Rate to scale 
		with the Aim Yaw Rate. This makes the character rotate faster when moving the camera faster.*/
		if (RotateL || RotateR)
		{
			//AAimingRotation;
			RotateRate = UKismetMathLibrary::MapRangeClamped(AAimYawRate,
				90.f /* Aim Yaw Rate Min Range*/,
				270.f /* Aim Yaw Rate Max Rang*/,
				1.15f /*Min Play Rate*/,
				3.0f/*Max Play Rate*/);
		}


	}
	else
	{
		RotateL = false;
		RotateR = false;
	}
}

bool UZS_AnimInstance::CanTurnInPlace()
{
	return AI_RotationMode == ERotationMode::ERM_LookingDirection && AI_ViewMode == EViewMode::EVM_ThirdPerson && GetCurveValue("Enable_Transition") > 0.99f;
}

void UZS_AnimInstance::TurnInPlaceCheck()
{
	if (CanTurnInPlace())
	{
		/*Step 1: Check if Aiming angle is outside of the Turn Check Min Angle, and if the Aim Yaw 
		Rate is below the Aim Yaw Rate Limit. If so, begin counting the Elapsed Delay Time. 
		If not, reset the Elapsed Delay Time. This ensures the conditions remain true
		for a sustained peroid of time before turning in place.*/

		float TurnCheckMinAngle = 45.f;
		float AimYawRateLimite = 50.f;
		float t_AimingAngle = abs(AimingAngle.X);
		if (t_AimingAngle > TurnCheckMinAngle && AAimYawRate < AimYawRateLimite)
		{
			ElapsedDelayTime += DeltaX;


			/*Step 2: Check if the Elapsed Delay time exceeds the set delay 
			(mapped to the turn angle range). If so, trigger a Turn In Place.*/

			float Cfloat = UKismetMathLibrary::MapRangeClamped(t_AimingAngle, TurnCheckMinAngle, 180.f, 0.75f /* min Angle Delay*/, 0.0f /*Max Angle Delay*/);

			if (ElapsedDelayTime > Cfloat)
			{
				//FRotator tempRot = FRotator(0.0f, AAimingRotation.Yaw, 0.0f);
				TurnInPlace(FRotator(0.0f, AAimingRotation.Yaw, 0.0f), 1.0f, 0.0f, false);
			}
		}
		else
		{
			ElapsedDelayTime = 0.0f;
			return;
		}
	}
	else
	{
		ElapsedDelayTime = 0.0f;
	}
}

void UZS_AnimInstance::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurent)
{
	//Step 1: Set Turn Angle
	float TurnAngle = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, PlayerCharacter->GetActorRotation()).Yaw;

	//Step 2: Choose Turn Asset based on the Turn Angle and Stance
	FTurnInPlace_Asset TargetTurnAsset;

	if (abs(TurnAngle) < 130.f/*Turn 180Threshold*/)
	{
		if (TurnAngle < 0.0f)
		{
			switch (AI_Stance)
			{
			case EStance::EMA_Standing:
				TargetTurnAsset = NTurnIPL90;
				break;
			case EStance::EMA_Crouching:
				TargetTurnAsset = CLFTurnIPL90;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (AI_Stance)
			{
			case EStance::EMA_Standing:
				TargetTurnAsset = NTurnIPR90;
				break;
			case EStance::EMA_Crouching:
				TargetTurnAsset = CLFTurnIPR90;
				break;
			default:
				break;
			}
		}
	}
	else
	{
		
		if (TurnAngle < 0.0f)
		{
			switch (AI_Stance)
			{
			case EStance::EMA_Standing:
				TargetTurnAsset = NTurnIPL180;
				break;
			case EStance::EMA_Crouching:
				TargetTurnAsset = CLFTurnIPL180;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (AI_Stance)
			{
			case EStance::EMA_Standing:
				TargetTurnAsset = NTurnIPR180;
				break;
			case EStance::EMA_Crouching:
			TargetTurnAsset = CLFTurnIPR180;
				break;
			default:
				break;
			}
		}
	}

	/*Step 3: If the Target Turn Animation is not playing or set to be overriden, 
	play the turn animation as a dynamic montage.*/
	if (OverrideCurent || !IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName))
	{
		PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, 0.2f, 0.2f, TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.0f, StartTime);

		/*Step 4: Scale the rotation amount (gets scaled in animgraph) to compensate for turn angle (If Allowed) and play rate.*/

		if (TargetTurnAsset.ScaleTurnAngle)
			RotationScale = (TurnAngle / TargetTurnAsset.AnimatedAngle) * TargetTurnAsset.PlayRate * PlayRateScale;
		else
			RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
	}

}

bool UZS_AnimInstance::CanDynamicallyTransition()
{
	/*Only perform a Dynamic Transition check if the "Enable Transition" curve is fully weighted.
	The Enable_Transition curve is modified within certain states of the AnimBP so that the character can only
	transition while in those states.*/
	return GetCurveValue("Enable_Transition") == 1.0f ? true : false;
}

void UZS_AnimInstance::PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams_Asset Parameters)
{
	/*Play Dynamic Additive Transition Animation*/

	FTimerHandle DelayTimer;
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &UZS_AnimInstance::MakeTheGateOpen, ReTriggerDelay, false);
	
	if (Gates.IsOpen())
	{
		PlaySlotAnimationAsDynamicMontage(Parameters.Animation, "Grounded Slot", Parameters.BlendInTime, Parameters.BlendOutTime, Parameters.PlayRate, 1, 0.0, Parameters.StartTime);
		
	}
	Gates.Close();

	
}

void UZS_AnimInstance::DynamicTranstionCheck()
{
	/*Check each foot to see if the location difference between the IK_Foot bone and its desired / target location
	(determined via a virtual bone) exceeds a threshold. If it does, play an additive transition animation on that foot.
	The currently set transition plays the second half of a 2 foot transition animation, so that only a single foot
	moves. Because only the IK_Foot bone can be locked, the separate virtual bone allows the system to know its
	desired location when locked.*/

	//float TempL = UKismetAnimationLibrary::K2_DistanceBetweenTwoSocketsAndMapRange(GetOwningComponent(), "ik_foot_l", ERelativeTransformSpace::RTS_Component, "VB foot_target_l", ERelativeTransformSpace::RTS_Component, false, 0.0f, 0.0f, 0.0f, 0.0f);

	FVector IKFootL = PlayerCharacter->GetMesh()->GetSocketLocation("ik_foot_l");
	FVector VBFootL = PlayerCharacter->GetMesh()->GetSocketLocation("VB foot_target_l");

	float TempL = FVector::Distance(IKFootL, VBFootL);
	
	if (TempL > 8.0f)
	{
		// PlayDynamic Transition
		FDynamicMontageParams_Asset tempR;
		tempR.Animation = N_Transitor_R;
		tempR.BlendInTime = 0.2f;
		tempR.BlendOutTime = 0.2f;
		tempR.PlayRate = 1.5f;
		tempR.StartTime = 0.8f;
		PlayDynamicTransition(0.1, tempR);

	}

	//float TempR = UKismetAnimationLibrary::K2_DistanceBetweenTwoSocketsAndMapRange(GetOwningComponent(), "ik_foot_r", ERelativeTransformSpace::RTS_Component, "VB foot_target_r", ERelativeTransformSpace::RTS_Component, false, 0.0f, 0.0f, 0.0f, 0.0f);



	FVector IKFootR = PlayerCharacter->GetMesh()->GetSocketLocation("ik_foot_r");
	FVector VBFootR = PlayerCharacter->GetMesh()->GetSocketLocation("VB foot_target_r");
	float TempR = FVector::Distance(IKFootR, VBFootR);
	if (TempR > 8.0f)
	{
		// PlayDynamic Transition
		FDynamicMontageParams_Asset tempL;
		tempL.Animation = N_Transitor_L;
		tempL.BlendInTime = 0.2f;
		tempL.BlendOutTime = 0.2f;
		tempL.PlayRate = 1.5f;
		tempL.StartTime = 0.8f;
		PlayDynamicTransition(0.1, tempL);
	}
}

void UZS_AnimInstance::MakeTheGateOpen()
{
	Gates.Open();
}

void UZS_AnimInstance::SetFootLocking(FName EnableFootIkCurve, FName FootLockCurve, FName IKFootBone, float& FootLockAlpha, FVector& FootLockLocation, FRotator& FootLockRotation)
{
	//Only update values if FootIK curve has a weight.
	if (GetCurveValue(EnableFootIkCurve) > 0.0f)
	{
		//Step 1: Set Local FootLock Curve value
		float FootLockCurveValue = GetCurveValue(FootLockCurve);
		
		/*Step 2: Only update the FootLock Alpha if the new value is less than the current, or it equals 1.
		This makes it so that the foot can only blend out of the locked position or lock to a new position,
		and never blend in.*/
		if (FootLockCurveValue >= 0.99f || FootLockCurveValue < FootLockAlpha)
		{
			FootLockAlpha = FootLockCurveValue;
		}

		/*Step 3: If the Foot Lock curve equals 1, save the new lock location and rotation in component space.*/
		if (FootLockAlpha >= 0.99f)
		{
			FootLockLocation = GetOwningComponent()->GetSocketTransform(IKFootBone, ERelativeTransformSpace::RTS_Component).GetLocation();
			FootLockRotation = GetOwningComponent()->GetSocketTransform(IKFootBone, ERelativeTransformSpace::RTS_Component).Rotator();
		}
		/*Step 4: If the Foot Lock Alpha has a weight, update the Foot Lock offsets to keep the foot planted in place
		while the capsule moves.*/
		if (FootLockAlpha > 0.0f)
		{
			SetFootLockOffsets(FootLockLocation, FootLockRotation);
		}
	}
}

void UZS_AnimInstance::SetFootOffset(FName EnableFootIkCurve, FName IKFootBone, FName RootBone, FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset)
{
	//Only update Foot IK offset values if the Foot IK curve has a weight. 
	// If it equals 0, clear the offset values.
	if (GetCurveValue(EnableFootIkCurve) > 0.0f)
	{
		//Step 1: Trace downward from the foot location to find the geometry. 
		// If the surface is walkable, save the Impact Location and Normal.

		FVector IKFootFloorLocation = UKismetMathLibrary::MakeVector(
			GetOwningComponent()->GetSocketLocation(IKFootBone).X, 
			GetOwningComponent()->GetSocketLocation(IKFootBone).Y, 
			GetOwningComponent()->GetSocketLocation(RootBone).Z);
		FVector Start = IKFootFloorLocation + FVector(0.0f, 0.0f, 50.f);
		FVector End = IKFootFloorLocation - FVector(0.0f, 0.0f, 45.f);
		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwningActor());

		UKismetSystemLibrary::LineTraceSingle(GetWorld(), 
			Start, 
			End, 
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore, 
			EDrawDebugTrace::ForOneFrame,
			HitResult, 
			true, FColor::Red,
			FColor::Green,
			5.0f);
		FRotator TargetRotationOffset;

		if (PlayerCharacter->GetCharacterMovement()->IsWalkable(HitResult))
		{
			FVector ImpactPoint = HitResult.ImpactPoint;
			FVector ImpactNormal = HitResult.ImpactNormal;

			/*Step 1.1: Find the difference in location from the Impact point and the expected (flat) 
			floor location.	These values are offset by the normally multiplied by the foot height to 
			get better behavior on angled surfaces.*/
			CurrentLocationTarget = (ImpactPoint + (ImpactNormal * FootHeight)) - (IKFootFloorLocation + (FVector(0.0f, 0.0f, 1.0f) * FootHeight));

			/*Step 1.2: Calculate the Rotation offset by getting the Atan2 of the Impact Normal.*/
			TargetRotationOffset = UKismetMathLibrary::MakeRotator(
				UKismetMathLibrary::Atan2(ImpactNormal.Y, ImpactNormal.Z), 
				UKismetMathLibrary::Atan2(ImpactNormal.X, ImpactNormal.Z) * -1.0f, 
				0.0f);

		}
		/*Step 2: Interp the Current Location Offset to the new target value. 
		Interpolate at different speeds based on whether the new target is above or 
		below the current one.*/
		if (CurrentLocationOffset.Z > CurrentLocationTarget.Z)
		{
			CurrentLocationOffset = UKismetMathLibrary::VInterpTo(
				CurrentLocationOffset, 
				CurrentLocationTarget, 
				DeltaX, 
				30.f);
		}
		else
		{
			CurrentLocationOffset = UKismetMathLibrary::VInterpTo(
				CurrentLocationOffset, 
				CurrentLocationTarget, 
				DeltaX, 
				15.f);
		}
		/*Step 3: Interp the Current Rotation Offset to the new target value.*/
		CurrentRotationOffset = UKismetMathLibrary::RInterpTo(
			CurrentRotationOffset, 
			TargetRotationOffset, 
			DeltaX, 
			30.f);

	}
	else
	{
		CurrentLocationOffset = FVector(0.0f);
		CurrentRotationOffset = FRotator(0.0f);
	}
}

void UZS_AnimInstance::SetPelvisIKOffset(FVector FootOffsetLTarget, FVector FootOffsetRTarget)
{
	/*Calculate the Pelvis Alpha by finding the average Foot IK weight. If the alpha is 0, clear the offset.*/
	PelvisAlpha = (GetCurveValue("Enable_FootIK_L") + 
		GetCurveValue("Enable_FootIK_R")) / 2.0f;

	if (PelvisAlpha > 0.0f)
	{
		/*Step 1: Set the new Pelvis Target to be the lowest Foot Offset*/

		if (FootOffsetLTarget.Z < FootOffsetRTarget.Z)
		{
			PelvisTarget = FootOffsetLTarget;
		}
		else
		{
			PelvisTarget = FootOffsetRTarget;
		}

		/*Step 2: Interp the Current Pelvis Offset to the new target value. 
		Interpolate at different speeds based on whether the new target is above or 
		below the current one.*/

		if (PelvisTarget.Z > PelvisOffset.Z)
		{
			PelvisOffset = UKismetMathLibrary::VInterpTo(
				PelvisOffset, PelvisTarget, DeltaX, 10.0f);
		}
		else
		{
			PelvisOffset = UKismetMathLibrary::VInterpTo(
				PelvisOffset, PelvisTarget, DeltaX, 15.0f);
		}

	}
	else
	{
		PelvisOffset = FVector(0.0f); 
	}

}

void UZS_AnimInstance::ResetIKOffsets()
{
	//Interp Foot IK offsets back to 0

	FootOffsetLLocation = UKismetMathLibrary::VInterpTo(FootOffsetLLocation, FVector(0.0f), DeltaX, 15.0f);
	FootLockRLocation = UKismetMathLibrary::VInterpTo(FootLockRLocation, FVector(0.0f), DeltaX, 15.0f);

	FootOffsetRRotation = UKismetMathLibrary::RInterpTo(FootOffsetRRotation, FRotator(0.0f), DeltaX, 15.0f);
	FootOffsetLRotation = UKismetMathLibrary::RInterpTo(FootOffsetLRotation, FRotator(0.0f), DeltaX, 15.0f);
}

void UZS_AnimInstance::SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation)
{
	//Use the delta between the current and last updated rotation to find how much the foot should be rotated to 
	//remain planted on the ground.
	FRotator RotationDifference;
	if (PlayerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		RotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(
			PlayerCharacter->GetActorRotation(), 
			PlayerCharacter->GetCharacterMovement()->GetLastUpdateRotation());
	}
	/*Get the distance traveled between frames relative to the mesh rotation to find how much the foot should be
	offset to remain planted on the ground.*/

	//FVector LocationDifference = UKismetMathLibrary::LessLess_VectorRotator(AVelocity * GetWorld()->GetDeltaSeconds(),GetOwningComponent()->GetComponentRotation());
	FVector LocationDifference = GetOwningComponent()->GetComponentRotation().UnrotateVector(
		AVelocity * GetWorld()->GetDeltaSeconds());

	/*Subtract the location difference from the current local location and rotate it by the rotation difference to
	keep the foot planted in component space.*/
	LocalLocation = UKismetMathLibrary::RotateAngleAxis(LocalLocation - LocationDifference, RotationDifference.Yaw, FVector(0.0f, 0.0f, -1.0f));

	/*Subtract the Rotation Difference from the current Local Rotation to get the new local rotation.*/
	LocalRotation = UKismetMathLibrary::NormalizedDeltaRotator(LocalRotation, RotationDifference);


}

void UZS_AnimInstance::UpdateInAirValues()
{
	/*Update the fall speed. Setting this value only while in the air allows you to use it within the
	AnimGraph for the landing strength. If not, the Z velocity would return to 0 on landing. */

	FallSpeed = AVelocity.Z;

	//Set the Land Prediction weight.

	LandPrediction = CalculateLandPrediction();

	/*Interp and set the In Air Lean Amount*/

	LeanAmount = InterpLeanAmount(LeanAmount, CalculateInAirLeanAmount(), 4.f, DeltaX);
}

float UZS_AnimInstance::CalculateLandPrediction()
{
	/*Calculate the land prediction weight by tracing in the velocity direction to find a walkable surface the
	character is falling toward, and getting the 'Time' (range of 0-1, 1 being maximum, 0 being about to land)
	till impact. The Land Prediction Curve is used to control how the time affects the final weight for a smooth
	blend. */
	if (FallSpeed < -200.f)
	{
		FVector TStart = PlayerCharacter->GetCapsuleComponent()->GetComponentLocation();
		float Radius = PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		float CapsuleHalfHeight = PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FVector Temp = FVector(AVelocity.X, AVelocity.Y, UKismetMathLibrary::Clamp(AVelocity.Z, -4000.f, -200.f));
		FVector Temp2 = UKismetMathLibrary::Vector_NormalUnsafe(Temp);
		float MRC = UKismetMathLibrary::MapRangeClamped(AVelocity.Z, 0.0f, -4000.f, 50.f, 2000.f);

		FVector TEnd = TStart + UKismetMathLibrary::Multiply_VectorFloat(Temp2, MRC);

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(PlayerCharacter);
		FHitResult Hit;

		UKismetSystemLibrary::CapsuleTraceSingleByProfile(GetWorld(), TStart, TEnd, Radius, CapsuleHalfHeight, TEXT("P_Character"), false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, Hit, true, FColor::Green, FColor::Red, 5.0f);

		if (PlayerCharacter->GetCharacterMovement()->IsWalkable(Hit) && Hit.bBlockingHit)
		{

			return UKismetMathLibrary::Lerp(LandPredictionCurve->GetFloatValue(Hit.Time), 0.0f, GetCurveValue("Mask_LandPrediction"));
		}
		else
		{
			return 0.0f;
		}


	}
	else
	{
		return 0.0f;
	}

	return 0.0f;
}

FLeanAmount UZS_AnimInstance::CalculateInAirLeanAmount()
{
	/*Use the relative Velocity direction and amount to determine how much the character
	should lean while in air. The Lean In Air curve gets the Fall Speed and is used as a multiplier to
	smoothly reverse the leaning direction when transitioning from moving upwards to moving downwards.*/

	FVector URotate = PlayerCharacter->GetActorRotation().UnrotateVector(AVelocity);
	FVector FallVec = UKismetMathLibrary::Divide_VectorFloat(URotate, 350.f);
	FVector2D temp = FVector2D(FallVec.Y, FallVec.X);
	float leanTime = LeanInAirCurve->GetFloatValue(FallSpeed);
	FVector2D MakeLeanAmount = UKismetMathLibrary::Multiply_Vector2DFloat(temp, leanTime);
	FLeanAmount LeanTemp{ MakeLeanAmount.X, MakeLeanAmount.Y };


	return LeanTemp;
}
