// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ZS_PlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ZS_API AZS_PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AZS_PlayerCameraManager();
	UFUNCTION(BlueprintCallable, Category = "Custom Camera")
	void OnPosses(APawn* NewPawn);

	UFUNCTION(BlueprintCallable, Category = "Custom Camera")
	void UpdateCustomCamera(FVector& Location, FRotator& Rotation, float& FOV);

	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

	// Utility
	float GtCameraBehaviorParam(FName CurrentName);
	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRtation, FVector LagSpeed);


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	APawn* ControlledPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	class UZS_CameraAnimInst* PCBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CameraMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FTransform PivotTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FTransform SmoothedPivotTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FRotator DebugViewRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FRotator TargetCameraRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FVector FPTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FVector PivotLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FVector TargetCameraLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	FVector DebugViewOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	float TPFOV;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	float FPFOV;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterInformation", meta = (AllowPrivateAccess = "true"))
	bool bRightShoulder;
};
