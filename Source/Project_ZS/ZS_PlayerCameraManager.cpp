// Fill out your copyright notice in the Description page of Project Settings.

#include "ZS_CameraAnimInst.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ZS_CameraInterface.h"
#include "ZS_PlayerCameraManager.h"

AZS_PlayerCameraManager::AZS_PlayerCameraManager():
	FPTarget(0.f),
	DebugViewRotation(0.f, -5.f, 180.f),
	TPFOV(0.f),
	FPFOV(0.f),
	TargetCameraRotation(0.f),
	bRightShoulder(false),
	DebugViewOffset(350.f, 0.f, 50.f),
	ControlledPawn(nullptr)
{
	CameraMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Camera"));
}

void AZS_PlayerCameraManager::OnPosses(APawn* NewPawn)
{
	/* Set Controlled Pawn when player controller Possesses new character. (Called from player Controller*/
	if (NewPawn != nullptr)
	{
		ControlledPawn = NewPawn;

		/* Update references in the cameraBehavior*/
		PCBehavior = Cast<UZS_CameraAnimInst>(CameraMesh->GetAnimInstance());
		if (PCBehavior)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, TEXT("PCBehavior"));
			PCBehavior->PlayerController = GetOwningPlayerController();
			PCBehavior->ControlledPawn = ControlledPawn;

		}

	}
}

void AZS_PlayerCameraManager::UpdateCustomCamera(FVector& Location, FRotator& Rotation, float& FOV)
{
	FVector TraceOrigins;
	float TraceRadius = 0.f;
	FVector ADSLoc;
	TEnumAsByte<ETraceTypeQuery> Trace;
	if (IZS_CameraInterface* CamInterface = Cast<IZS_CameraInterface>(ControlledPawn))
	{
		// Get the camera parameters from character class via interface
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, TEXT("CamInterface Valid"));
		PivotTarget = CamInterface->Execute_Get3PPivotTarget(ControlledPawn);
		CamInterface->Execute_GetFPCameraTarget(ControlledPawn, FPTarget);
		CamInterface->Execute_GetCameraParameters(ControlledPawn, TPFOV, FPFOV, bRightShoulder);
		CamInterface->Execute_Get3PParams(ControlledPawn, TraceOrigins, TraceRadius, Trace);
		CamInterface->Execute_AimDownSight(ControlledPawn, ADSLoc); // 31/01/2024
	}

	//Step 2: Calculate Target Camera Rotation. Use the Control Rotation and interpolate for smooth camera rotation.
	FRotator Interpret = UKismetMathLibrary::RInterpTo(GetCameraRotation(), GetOwningPlayerController()->GetControlRotation(), GetWorld()->GetDeltaSeconds(), GtCameraBehaviorParam(FName("RotationLagSpeed")));
	TargetCameraRotation = UKismetMathLibrary::RLerp(Interpret, DebugViewRotation, GtCameraBehaviorParam("Override_Debug"), true);

	/*Step 3: Calculate the Smoothed Pivot Target (Orange Sphere).
	Get the 3P Pivot Target (Green Sphere) and interpolate using axis independent
	lag for maximum control.*/

	FVector LagSpeeds = UKismetMathLibrary::MakeVector(GtCameraBehaviorParam("PivotLagSpeed_X"), GtCameraBehaviorParam("PivotLagSpeed_Y"), GtCameraBehaviorParam("PivotLagSpeed_Z"));
	FVector CAxis = CalculateAxisIndependentLag(SmoothedPivotTarget.GetLocation(), PivotTarget.GetLocation(), TargetCameraRotation, LagSpeeds);
	SmoothedPivotTarget = UKismetMathLibrary::MakeTransform(CAxis, PivotTarget.Rotator(), FVector(1.f));

	/*Step 4: Calculate Pivot Location (BlueSphere). Get the Smoothed Pivot Target and
	apply local offsets for further camera control.*/

	FVector SmFVector = UKismetMathLibrary::GetForwardVector(SmoothedPivotTarget.Rotator()) * GtCameraBehaviorParam("PivotOffset_X");
	FVector SmRVector = UKismetMathLibrary::GetRightVector(SmoothedPivotTarget.Rotator()) * GtCameraBehaviorParam("PivotOffset_Y");
	FVector SmUVector = UKismetMathLibrary::GetUpVector(SmoothedPivotTarget.Rotator()) * GtCameraBehaviorParam("PivotOffset_Z");

	PivotLocation = SmoothedPivotTarget.GetLocation() + SmFVector + SmRVector + SmUVector;

	/*Step 5: Calculate Target Camera Location.
	Get the Pivot location and apply camera relative offsets.*/

	//FVector TCRFVector = UKismetMathLibrary::GetForwardVector(TargetCameraRotation) * GtCameraBehaviorParam("CameraOffset_X");
	//FVector TCRRVector = UKismetMathLibrary::GetRightVector(TargetCameraRotation) * GtCameraBehaviorParam("CameraOffset_Y");
	//FVector TCRZVector = UKismetMathLibrary::GetUpVector(TargetCameraRotation) * GtCameraBehaviorParam("CameraOffset_Z");

	FVector TCRFVector = UKismetMathLibrary::Multiply_VectorFloat(UKismetMathLibrary::GetForwardVector(TargetCameraRotation), GtCameraBehaviorParam("CameraOffset_X"));
	FVector TCRRVector = UKismetMathLibrary::Multiply_VectorFloat(UKismetMathLibrary::GetRightVector(TargetCameraRotation), GtCameraBehaviorParam("CameraOffset_Y"));
	FVector TCRZVector = UKismetMathLibrary::Multiply_VectorFloat(UKismetMathLibrary::GetUpVector(TargetCameraRotation), GtCameraBehaviorParam("CameraOffset_Z"));

	FVector PivotSum = PivotLocation + TCRFVector + TCRRVector + TCRZVector;
	FVector PVotDebug = PivotTarget.GetLocation() + DebugViewOffset;
	TargetCameraLocation = UKismetMathLibrary::VLerp(PivotSum, PVotDebug, GtCameraBehaviorParam("Override_Debug"));

	/* Step 6: Trace for an object between the camera and character to apply a corrective offset.
	Trace origins are set within the Character BP via the Camera Interface. Functions like the
	normal spring arm, but can allow for different trace origins regardless of the pivot. */

	TArray<AActor*> IgActor;
	//IgActor.Add(ControlledPawn);
	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceOrigins, TargetCameraLocation, TraceRadius, Trace, false, IgActor, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 0.f);
	if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
	{
		TargetCameraLocation += (HitResult.Location - HitResult.TraceEnd);
	}

	/*Step 7: Draw Debug Shapes.*/
	/*UKismetSystemLibrary::DrawDebugSphere(GetWorld(), PivotTarget.GetLocation(), 16.f, 8, FLinearColor::Green, 0.0f, 0.5f);
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), SmoothedPivotTarget.GetLocation(), 16.f, 8, FLinearColor::Yellow, 0.0f, 0.5f);
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), PivotLocation, 16.f, 8, FLinearColor::Red, 0.f, 0.5f);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), SmoothedPivotTarget.GetLocation(), PivotTarget.GetLocation(), FLinearColor::Blue, 0.0f, 3.f);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), PivotLocation, SmoothedPivotTarget.GetLocation(), FLinearColor::Black, 0.0f, 3.f);*/

	/*Step 8: Lerp First Person Override and return target camera parameters.*/
	FTransform TP = UKismetMathLibrary::MakeTransform(TargetCameraLocation, TargetCameraRotation, FVector(1.f));
	FTransform FP = UKismetMathLibrary::MakeTransform(FPTarget, TargetCameraRotation, FVector(1.f));
	FTransform DB = UKismetMathLibrary::MakeTransform(TargetCameraLocation, DebugViewRotation, FVector(1.f));



	FTransform ADS = UKismetMathLibrary::MakeTransform(ADSLoc, TargetCameraRotation, FVector(1.f));
	FTransform ADST = UKismetMathLibrary::TLerp(FP, ADS, GtCameraBehaviorParam("Weight_ADS"));



	FTransform LerpFOV = UKismetMathLibrary::TLerp(TP, ADST, GtCameraBehaviorParam("Weight_FirstPerson"));
	FTransform LerpDebug = UKismetMathLibrary::TLerp(LerpFOV, DB, GtCameraBehaviorParam("Override_Debug"));
	float FLerpFOV = UKismetMathLibrary::Lerp(TPFOV, FPFOV, GtCameraBehaviorParam("Weight_FirstPerson"));


	//9 Set Values
	Location = LerpDebug.GetLocation();
	Rotation = LerpDebug.Rotator();
	FOV = FLerpFOV;

}

void AZS_PlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTargetInternal(OutVT, DeltaTime);

	if (OutVT.Target->ActorHasTag(FName("ZS_Character")))
	{
		UpdateCustomCamera(OutVT.POV.Location, OutVT.POV.Rotation, OutVT.POV.FOV);
	}

}

float AZS_PlayerCameraManager::GtCameraBehaviorParam(FName CurrentName)
{
	/*Get an Anim Curve value from the Player Camera Behavior AnimBP to use
	as a parameter in the custom camera behavior calculations*/

	if (CameraMesh->GetAnimInstance())
	{
		return CameraMesh->GetAnimInstance()->GetCurveValue(CurrentName);
	}
	else
	{
		return 0.0f;
	}
}

FVector AZS_PlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRtation, FVector LagSpeed)
{
	FRotator CameraRotationYaw = FRotator(0.0f, 0.0f, CameraRtation.Yaw);

	FVector UnrotVec1 = UKismetMathLibrary::LessLess_VectorRotator(CurrentLocation, CameraRotationYaw);
	FVector UnrotVec2 = UKismetMathLibrary::LessLess_VectorRotator(TargetLocation, CameraRotationYaw);

	float X = UKismetMathLibrary::FInterpTo(UnrotVec1.X, UnrotVec2.X, GetWorld()->GetDeltaSeconds(), LagSpeed.X);
	float Y = UKismetMathLibrary::FInterpTo(UnrotVec1.Y, UnrotVec2.Y, GetWorld()->GetDeltaSeconds(), LagSpeed.Y);
	float Z = UKismetMathLibrary::FInterpTo(UnrotVec1.Z, UnrotVec2.Z, GetWorld()->GetDeltaSeconds(), LagSpeed.Z);

	return CameraRotationYaw.RotateVector(UKismetMathLibrary::MakeVector(X, Y, Z));

}
