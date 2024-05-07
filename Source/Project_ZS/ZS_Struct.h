#pragma once
#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "ZS_Struct.generated.h"



USTRUCT(BlueprintType)
struct FMovementSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float WalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float RunSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float SprintSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    UCurveVector* MovementCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    UCurveFloat* RotationCurve;



};

USTRUCT(BlueprintType)
struct FMovementSettings_Stance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    FMovementSettings Standing;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    FMovementSettings Crouching;
};

USTRUCT(BlueprintType)
struct FMovementSettings_State : public FTableRowBase //  FTableRowBase Allows to create data table based on this struct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    FMovementSettings_Stance VelocityDirection;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    FMovementSettings_Stance LookingDirection;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    FMovementSettings_Stance Aiming;
};



USTRUCT(BlueprintType)
struct FMantle_TraceSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float MaxLedgeHeight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float MinLedgeHeight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float ReachDistance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float ForwardTraceRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float DownWardTraceRadius;


};

USTRUCT(BlueprintType)
struct FMantle_Params
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    UAnimMontage* AnimMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    UCurveVector* PositionCurve;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float StartingPosition;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float PlayRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    FVector StartingOffset;


};

USTRUCT(BlueprintType)
struct FComponentAndTransform
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    FTransform Transform;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    UPrimitiveComponent* Component;


};

USTRUCT(BlueprintType)
struct FMantle_Assets
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    UAnimMontage* AnimMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    UCurveVector* PositionCurve;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    FVector StartingOffset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float LowHeight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float LowPlayRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float LowStartPosition;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float HighHeight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float HightPlayRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantle")
    float HighStartPosition;



};

USTRUCT(BlueprintType)
struct FVelocityBlend
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityBlend")
    float F;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityBlend")
    float B;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityBlend")
    float L;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityBlend")
    float R;
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LeanAmount")
    float LR;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LeanAmount")
    float FB;

};

USTRUCT(BlueprintType)
struct FTurnInPlace_Asset
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FTurnInPlace_Asset")
    UAnimSequence* Animation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FTurnInPlace_Asset")
    float AnimatedAngle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FTurnInPlace_Asset")
    FName SlotName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FTurnInPlace_Asset")
    float PlayRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FTurnInPlace_Asset")
    bool ScaleTurnAngle;

};

USTRUCT(BlueprintType)
struct FDynamicMontageParams_Asset
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDynamicMontageParams_Asset")
    UAnimSequence* Animation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDynamicMontageParams_Asset")
    float BlendInTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDynamicMontageParams_Asset")
    float BlendOutTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDynamicMontageParams_Asset")
    float PlayRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDynamicMontageParams_Asset")
    float StartTime;



};

USTRUCT(BlueprintType)
struct FGate
{
    GENERATED_BODY()

public:
    FORCEINLINE FGate();
    explicit FORCEINLINE FGate(const bool bStartClosed);

    FORCEINLINE void Open() { bGateOpen = true; }
    FORCEINLINE void Close() { bGateOpen = false; }
    FORCEINLINE void Toggle() { bGateOpen = !bGateOpen; }
    FORCEINLINE bool IsOpen() const { return bGateOpen; }


private:
    UPROPERTY(VisibleAnywhere)
    bool bGateOpen;



};

FORCEINLINE FGate::FGate() : bGateOpen(false) {}
FORCEINLINE FGate::FGate(const bool bStartClosed) : bGateOpen(!bStartClosed) {}




USTRUCT(BlueprintType)
struct FDoOnce
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bDoOnce;


    FORCEINLINE FDoOnce();
    explicit FORCEINLINE FDoOnce(const bool bDoOne);


    FORCEINLINE void Reset() { bDoOnce = true; }
    FORCEINLINE bool Execute()
    {
        if (bDoOnce)
        {
            bDoOnce = false;
            return true;
        }
        else
        {
            return false;
        }

    }
};

FORCEINLINE FDoOnce::FDoOnce() : bDoOnce(false) {}
FORCEINLINE FDoOnce::FDoOnce(const bool bDoOne) : bDoOnce(!bDoOne) {}


USTRUCT(BlueprintType)
struct FHoldingObject
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helding Object")
    UStaticMesh* NewStaticMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helding Object")
    USkeletalMesh* NewSkeletalMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helding Object")
    UClass* NewAnimClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helding Object")
    bool LeftHand;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helding Object")
    FVector Offset;
};