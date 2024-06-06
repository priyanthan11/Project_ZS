// Fill out your copyright notice in the Description page of Project Settings.


#include "ZS_CameraAnimInst.h"
#include "ZS_CameraInterface.h"
#include "ZS_Interface.h"

UZS_CameraAnimInst::UZS_CameraAnimInst() :
	//PawnMovementMode(&EMovementMode::MOVE_Walking), // How to Initialize
	MovementState(EMovementState::EMS_NONE),
	MovementAction(EMovementAction::EMA_NONE),
	RotationMode(ERotationMode::ERM_VelocityDirection),
	Gait(EGait::EGT_Walking),
	Stance(EStance::EMA_Standing),
	bRightShoulder(false)
{
}

void UZS_CameraAnimInst::UpdateCharacterInformation()
{
	if (ControlledPawn)
	{
		IZS_Interface* ICharacter = Cast<IZS_Interface>(ControlledPawn);
		ICharacter->Execute_GetCurrentStates(ControlledPawn, PawnMovementMode, MovementState, PrevMovementState, MovementAction, RotationMode, Gait, Stance, ViewMode, OverlayState);

		float TPFOV;
		float FPFOV;
		IZS_CameraInterface* ICamera = Cast<IZS_CameraInterface>(ControlledPawn);
		ICamera->Execute_GetCameraParameters(ControlledPawn, TPFOV, FPFOV, bRightShoulder);

		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, TEXT("AnimInstance ControlledPawn"));
	}
}

void UZS_CameraAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	UpdateCharacterInformation();
}
