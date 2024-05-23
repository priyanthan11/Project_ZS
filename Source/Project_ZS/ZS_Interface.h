// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZS_Enums.h"
#include "ZS_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UZS_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_ZS_API IZS_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player States")
	void SetMovementState(EMovementState NewMovementState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player States")
	void SetMovementAction(EMovementAction NewMovementAction);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player States")
	void SetGait(EGait NewGait);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Essential Values")
	void GetEssentialValues(FVector& IVelocity, FVector& IAcceleration, FVector& IMovementInput, bool& IIsMoving, bool& IHasMovementInput, float& ISpeed, float& IMovementInputAmount, float& IAimYawRate, FRotator& IAimingRotation);

	//void GetCurrentStates(EMovementMode& PawnMovementMode, EMovementState& MovementStates, EMovementState& PrevMovementState, EMovementAction& MovemeentAction, ERotationMode& RotationModes, EGait& ActualGaits, EStance& ActualStance, EViewMode& ViewModes);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Essential Values")
	void GetCurrentStates(TEnumAsByte<EMovementMode>& PawnMovementMode, EMovementState& MovementStates, EMovementState& PrevMovementStates, EMovementAction& MovemeentAction, ERotationMode& RotationModes, EGait& ActualGaits, EStance& ActualStance, EViewMode& ViewModes, EOverlayState& Overlays);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Essential Values")
	void SetOverlayState(EOverlayState Overlays);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Essential Values")
	void SetRotationMode(ERotationMode Rotations);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Essential Values")
	void SetViewMode(EViewMode ViewModes);
};
