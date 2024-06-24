// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma region Include
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZS_Interface.h"
#include "ZS_Struct.h"
#include "ZS_AnimInstance.generated.h"
#pragma endregion
/**
 * 
 */
UCLASS()
class PROJECT_ZS_API UZS_AnimInstance : public UAnimInstance, public IZS_Interface
{
	GENERATED_BODY()
	
public:
	UZS_AnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
	virtual void NativeInitializeAnimation() override;

protected:
#pragma region Essential Functions
	void UpdateCharacterInfor();
	void UpdateAimingValues();
	void UpdateLayerValues();
	void UpdateFootIK();

	void DoWhileMoving();
	void DoWhileNotMoving();

	float GetAnimCurve(FName Name);
	void DoWhileLoop();
#pragma endregion
#pragma region Grounded
	void UpdateMovementValues();
	void UpdateRotationValues();
#pragma region AssitFucntions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PureFunctions")
	FVelocityBlend CalculateVelocityBlend();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PureFunctions")
	FVelocityBlend InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target, float InterpSpeed, float DeltaTime);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PureFunctions")
	FLeanAmount InterpLeanAmount(FLeanAmount LA, FLeanAmount Target, float InterpSpeed, float Deltax);

	bool ShouldMoveCheck();
	float CalculateDiagonalScaleAmount();
	float CalculateStrideBlend();
	float CalculateWalkRunBlend();
	float CalculateStandingPlayRate();
	float CalculateCrouchingPlayRate();
	FVector CalculateRelativeAccelerationAmount();
	float GetAnimCuveClaped(FName Name, float Bias, float ClampMin, float ClampMax);
	EMovementDirection CalculateMovementDirection();
	EMovementDirection	CalculateQuadrant(EMovementDirection MovementDir, float FR_Threshold, float FL_Threshold, float BR_Threshold, float BL_Threshold, float Buffer, float Angle);
	bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float buffer, bool IncreaseBuffer);




#pragma endregion
#pragma region Do While Not Moving (F)

	bool CanRotateInPlace();
	void RotateInPlaceCheck();

	bool CanTurnInPlace();
	void TurnInPlaceCheck();
	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurent);

	bool CanDynamicallyTransition();
	void PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams_Asset Parameters);
	void DynamicTranstionCheck();
	void MakeTheGateOpen();

	void SetFootLocking(FName EnableFootIkCurve, FName FootLockCurve, FName IKFootBone, float& FootLockAlpha,FVector& FootLocation,FRotator& FootLockRotation);
	void SetFootOffset(FName EnableFootIkCurve, FName IKFootBone, FName RootBone,FVector& CurrentLocationTarget,FVector& CurrentLocationOffset,FRotator& CurrentRotationOffset );
	void SetPelvisIKOffset(FVector FootOffsetLTarget,FVector FootOffsetRTarget);
	void ResetIKOffsets();

#pragma endregion
#pragma endregion
#pragma region InAir
	void UpdateInAirValues();
	float CalculateLandPrediction();
	FLeanAmount CalculateInAirLeanAmount();
#pragma endregion
private:
#pragma region Reference Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	class AProject_ZSCharacter* PlayerCharacter;
	IZS_Interface* PlayerInterface;
#pragma endregion
#pragma region System Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "System Variable", meta = (AllowPrivateAccess = "true"))
	float DeltaX;
#pragma endregion
#pragma region Essential Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	FVector AVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	FVector AAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	FVector AMovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	bool bAIsMoving;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	bool bAHasMovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	float ASpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	float AMovementInputAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	FRotator AAimingRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Information", meta = (AllowPrivateAccess = "true"))
	float AAimYawRate;
#pragma endregion
#pragma region State Variables

	// -------------------------  Essential States from Character -----------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EMovementMode> AI_MovementMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EMovementState AI_MovementState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EMovementState AI_PrevMovementState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EMovementAction AI_MovementAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	ERotationMode AI_RotationMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EGait AI_Gait;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EStance AI_Stance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EViewMode AI_ViewMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EOverlayState AI_OverlayState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Values", meta = (AllowPrivateAccess = "true"))
	EGroundEntryState AI_GroundEntryState;
#pragma endregion
#pragma region AimingValues Variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	FRotator SmoothedAimingRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	FRotator SpineRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	FVector2D AimingAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	FVector2D SmoothedAimingAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	float AimSweepTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	float InputYawOffsetTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	float LeftYawTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	float RightYawTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimingValues", meta = (AllowPrivateAccess = "true"))
	float ForwardYawTime;
#pragma endregion
#pragma region UpdateLayerValues
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float EnableAimOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float BasePoseN;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float BasePoseCLF;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float SpineAdd;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float HeadAdd;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float ArmLAdd;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float ArmRAdd;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float HandR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float HandL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float EnableHandIKL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float EnableHandIKR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float ArmLLS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float ArmRLS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float ArmLMS;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UpdateLayerValues", meta = (AllowPrivateAccess = "true"))
	float ArmRMS;
	
#pragma endregion
#pragma region DoWhileLoop
	bool Condition;
	bool PreviousCondtion;
#pragma endregion
#pragma region Grounded
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grounded Values", meta = (AllowPrivateAccess = "true"))
	bool ShouldMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grounded Values", meta = (AllowPrivateAccess = "true"))
	bool Pivot;
#pragma region UpdateMovementValues
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FVelocityBlend VelocityBlend;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float DiagonalScaleAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	FVector RelativeAccelerationAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FLeanAmount LeanAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float WalkRunBlend;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float StrideBlend;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float StandingPlayRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float CrouchingPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Graph - Grounded|Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DiagonalScaleAmountCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Graph - Grounded|Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlendNWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Graph - Grounded|Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlendNRun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Graph - Grounded|Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlendCWalk;

#pragma endregion
#pragma region UpdateRotationValues
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	EMovementDirection MovementDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float FYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float BYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float LYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float RYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffsetFB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffsetLR;
#pragma endregion
#pragma region Do While Not Moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grounded Values - Not Moving", meta = (AllowPrivateAccess = "true"))
	bool RotateL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grounded Values - Not Moving", meta = (AllowPrivateAccess = "true"))
	bool RotateR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grounded Values - Not Moving", meta = (AllowPrivateAccess = "true"))
	float RotateRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grounded Values - Not Moving", meta = (AllowPrivateAccess = "true"))
	float ElapsedDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset NTurnIPL90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLFTurnIPL90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset NTurnIPR90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLFTurnIPR90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset NTurnIPL180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLFTurnIPL180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset NTurnIPR180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct", meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLFTurnIPR180;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	float RotationScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimSeq", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* N_Transitor_R;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimSeq", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* N_Transitor_L;


	FGate Gates;
#pragma endregion
#pragma region InAir Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - InAIr", meta = (AllowPrivateAccess = "true"))
	float LandPrediction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - InAIr", meta = (AllowPrivateAccess = "true"))
	float FallSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Graph - InAIr", meta = (AllowPrivateAccess = "true"))
	float JumpPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* LandPredictionCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* LeanInAirCurve;
#pragma endregion
#pragma region FootIK Varaiables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	float FootLockLAlpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FVector FootLockLLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FRotator FootLockLRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	float FootLockRAlpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FVector FootLockRLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FRotator FootLockRRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FVector FootOffsetLLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FRotator FootOffsetLRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FVector FootOffsetRLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimGraph - FookIK", meta = (AllowPrivateAccess = "true"))
	FRotator FootOffsetRRotation;

#pragma endregion
#pragma region Struct Values
	FDoOnce DoOnce;
	FDoOnce DoOnce1;
#pragma endregion
#pragma region EnumValues
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "true"))
	EHipDirection TrackedHipDirection;
#pragma endregion
};
