// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "ZS_Enums.h"

#include "ZS_CameraAnimInst.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ZS_API UZS_CameraAnimInst : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UZS_CameraAnimInst();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References", meta = (AllowPrivateAccess = "true"))
	APawn* ControlledPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References", meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController;

	UFUNCTION(BlueprintCallable)
	void UpdateCharacterInformation();

	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EMovementMode> PawnMovementMode = EMovementMode::MOVE_Walking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EMovementState MovementState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EMovementState PrevMovementState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EMovementAction MovementAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	ERotationMode RotationMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EGait Gait;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EStance Stance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EViewMode ViewMode;

	EOverlayState OverlayState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	bool bRightShoulder;

};
