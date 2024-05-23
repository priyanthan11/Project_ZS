// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

//#include "ZS_Enums.h"
#include "ZS_Interface.h"
#include "ZS_Struct.h"


#include "Project_ZSCharacter.generated.h"

template<typename T>
void SetPreviousAndNewValue(T NewValue, T& NewTarget, T& PreviousTarget)
{
	PreviousTarget = NewTarget;
	NewTarget = NewValue;
}


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProject_ZSCharacter : public ACharacter, public IZS_Interface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ISprintAction;
	/** Walk Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IWalkAction;
	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ICrouchAction;

	// Interface
	IZS_Interface* ZS_Interface;

#pragma region Essential Varaiables
	// Essential Varaibales
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	FVector Acceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	FVector PreviousVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	float MovementInputAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	float PreviousAimYaw;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	float AimYawRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	bool bIsMoving;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	bool bHasMovementInput;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	FRotator LastVelocityRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Essential Variables", meta = (AllowPrivateAccess = "true"))
	FRotator LastMovementInputRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation Variables", meta = (AllowPrivateAccess = "true"))
	FRotator TargetRotation;

#pragma endregion
#pragma region Enum Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	EMovementState MovementState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	EMovementState PrevMovementState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	EMovementAction MovementAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	ERotationMode RotationMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Variables", meta = (AllowPrivateAccess = "true"))
	ERotationMode DesiredRotationMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	EGait Gait;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	EStance Stance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Variables", meta = (AllowPrivateAccess = "true"))
	EStance DesiredStance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	EViewMode ViewMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum Variables", meta = (AllowPrivateAccess = "true"))
	EOverlayState OverlayState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Variables(Input)", meta = (AllowPrivateAccess = "true"))
	EGait DesiredGait;

#pragma endregion
#pragma region Struct Varaibles
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Struct Variables", meta = (AllowPrivateAccess = "true"))
	FMovementSettings CurrentMovementSettings;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Struct Variables", meta = (AllowPrivateAccess = "true"))
	FMovementSettings_State* MovementData;


#pragma endregion
public:
	AProject_ZSCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
#pragma region Essential Functions		
	// Essential Values
	UFUNCTION(BlueprintCallable, Category = "Essential Functions")
	void SetEssentialValues();
	UFUNCTION(BlueprintCallable, Category = "Essential Functions")
	void CacheValues();
	UFUNCTION(BlueprintCallable, Category = "Essential Functions")
	void DrawDebugShapes();
	void DrawVelocityArrow();
	void DrawMovementInputArror();
	void DrawTargetRotationArrow();
	void DrawAimingCone();
	void DrawCapsule();

	// Essential Pure / Helper Functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Essential Functions")
	FVector CalculateAcceleration();
#pragma endregion
#pragma region DO while on Ground - Movement
	
	UFUNCTION(BlueprintCallable, Category = "While On Ground - Movement")
	void UpdateCharacterMovement();
	UFUNCTION(BlueprintCallable, Category = "While On Ground - Movement")
	void UpdateGroundedRotation();

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
#pragma region Helper Functions
	// Update Character Movement related Fucntions
	UFUNCTION(BlueprintPure,BlueprintCallable, Category = "While On Ground - Movement")
	EGait GetAllowedGait();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "While On Ground - Movement")
	EGait GetActualGait(EGait m_AllowedGait);
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "While On Ground - Movement")
	bool CanSprint();
	UFUNCTION(BlueprintCallable, Category = "While On Ground - Movement")
	void UpdateDynamicMovementSttings(EGait m_AllowedGait);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "While On Ground - Movement")
	FMovementSettings GetTargetMovementSettings();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "While On Ground - Movement")
	float GetMappedSpeed();
#pragma endregion

#pragma endregion
#pragma region Do While On Ground - Rotation
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "While On Ground - Rotation")
	bool CanUpdateMovingRotation();
	UFUNCTION(BlueprintCallable,  Category = "While On Ground - Rotation")
	void SmoothChracterRotation(FRotator Target, float TargetInterp, float ActorInterpSpeed);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "While On Ground - Rotation")
	float CalculateGroundRotationRate();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "While On Ground - Rotation")
	float GetAnimCurveValue(FName CurveName);
	UFUNCTION(BlueprintCallable, Category = "While On Ground - Rotation")
	void LimitRotation(float AnimYawMin, float AnimYawMax, float InterpSpeed);
	void RotationAmout();
#pragma endregion
#pragma region Interface Fucntions
	virtual void SetGait_Implementation(EGait NewGait) override;
	virtual void SetMovementState_Implementation(EMovementState NewMovementState)override;
	virtual void GetCurrentStates_Implementation(TEnumAsByte<EMovementMode>&PawnMovementMode,
		EMovementState & MovementStates, 
		EMovementState & PrevMovementState, 
		EMovementAction & MovemeentAction, 
		ERotationMode & RotationModes, 
		EGait & ActualGaits, 
		EStance & ActualStance, 
		EViewMode & ViewModes, 
		EOverlayState & Overlays)override;
	virtual void GetEssentialValues_Implementation(FVector & IVelocity, 
		FVector & IAcceleration, FVector & IMovementInput, bool& IIsMoving, 
		bool& IHasMovementInput, float& ISpeed, float& IMovementInputAmount, float& IAimYawRate, FRotator & IAimingRotation) override;

#pragma endregion
#pragma region Value Change
	void OnMovementStateChanged(EMovementState NewMovementState);
	void OnGaitChanged(EGait NewActualGait);
	void OnRotationModeChanged(ERotationMode NewRotationMode);
	void OnStanceChanged(EStance NewStance);
#pragma endregion
#pragma region Input
	UFUNCTION(BlueprintCallable)
	void SprintAction(TEnumAsByte<EGait> t_DesirdGait);
	void SprintPressed();
	void SprintReleased();
	void StanceAction();
	UFUNCTION(BlueprintCallable)
	void WalkAction();
	
#pragma endregion

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	void Initialize_ZSValues();
	virtual void Tick(float DeltaSeconds) override;
	/**
	 * Called from CharacterMovementComponent to notify the character that the movement mode has changed.
	 * @param	PrevMovementMode	Movement mode before the change
	 * @param	PrevCustomMode		Custom mode before the change (applicable if PrevMovementMode is Custom)
	 */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

