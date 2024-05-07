#pragma once

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_NONE		UMETA(DisplayName = "None"),
	EMS_Grounded	UMETA(DisplayName = "Grounded"),
	EMS_InAir		UMETA(DisplayName = "InAir"),
	EMS_Mantling	UMETA(DisplayName = "Mantling"),
	EMS_Ragdoll		UMETA(DisplayName = "Ragdoll"),

};

UENUM(BlueprintType)
enum class EMovementAction : uint8
{
	EMA_NONE		UMETA(DisplayName = "None"),
	EMA_LowMantle	UMETA(DisplayName = "LowMantle"),
	EMA_HightMantle UMETA(DisplayName = "HighMantle"),
	EMA_Rolling		UMETA(DisplayName = "Rolling"),
	EMA_GettingUp	UMETA(DisplayName = "GettingUp")
};

UENUM(BlueprintType)
enum class EStance : uint8
{
	EMA_Standing		UMETA(DisplayName = "Standing"),
	EMA_Crouching		UMETA(DisplayName = "Crouching")

};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	ERM_VelocityDirection		UMETA(DisplayName = "VelocityDirection"),
	ERM_LookingDirection		UMETA(DisplayName = "LookingDirection"),
	ERM_Aiming					UMETA(DisplayName = "Aiming")

};

UENUM(BlueprintType)
enum class EGait : uint8
{
	EGT_Walking		UMETA(DisplayName = "Walking"),
	EGT_Running		UMETA(DisplayName = "Running"),
	EGT_Sprinting	UMETA(DisplayName = "Sprinting")

};

UENUM(BlueprintType)
enum class EViewMode : uint8
{
	EVM_ThirdPerson		UMETA(DisplayName = "ThirdPerson"),
	EVM_FirstPerson		UMETA(DisplayName = "FirstPerson")


};

UENUM(BlueprintType)
enum class EMantleType : uint8
{
	EMT_HighMantle		UMETA(DisplayName = "HighMantle"),
	EMT_LowMantle		UMETA(DisplayName = "LowMantle"),
	EMT_FallingCatch	UMETA(DisplayName = "FallingCatch")

};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	EMD_Forward		UMETA(DisplayName = "Forward"),
	EMD_Right		UMETA(DisplayName = "Right"),
	EMD_Left 		UMETA(DisplayName = "Left"),
	EMD_Backward 	UMETA(DisplayName = "Backward")

};

UENUM(BlueprintType)
enum class EHipDirection : uint8
{
	EHD_F		UMETA(DisplayName = "F"),
	EHD_B		UMETA(DisplayName = "B"),
	EHD_RF		UMETA(DisplayName = "RF"),
	EHD_RB		UMETA(DisplayName = "RB"),
	EHD_LF		UMETA(DisplayName = "LF"),
	EHD_LB		UMETA(DisplayName = "LB")

};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	EOS_Default		UMETA(DisplayName = "Default"),
	EOS_Masculine	UMETA(DisplayName = "Masculine"),
	EOS_Feminine	UMETA(DisplayName = "Feminine"),
	EOS_Injured		UMETA(DisplayName = "Injured"),
	EOS_HandsTied	UMETA(DisplayName = "HandsTied"),
	EOS_Rifle		UMETA(DisplayName = "Rifle"),
	EOS_Pistol_1H	UMETA(DisplayName = "Pistol_1H"),
	EOS_Pistol_2H	UMETA(DisplayName = "Pistol_2H"),
	EOS_Bow			UMETA(DisplayName = "Bow"),
	EOS_Torch		UMETA(DisplayName = "Torch"),
	EOS_Binoculars	UMETA(DisplayName = "Binoculars"),
	EOS_Box			UMETA(DisplayName = "Box"),
	EOS_Barrel		UMETA(DisplayName = "Barrel")

};

UENUM(BlueprintType)
enum class EGroundEntryState : uint8
{
	EGE_None		UMETA(DisplayName = "None"),
	EGE_Roll		UMETA(DisplayName = "Roll")


};

UENUM(BlueprintType)
enum class EFootStepType : uint8
{
	EFT_Step		UMETA(DisplayName = "Step"),
	EFT_Walk_Run	UMETA(DisplayName = "Walk/Run"),
	EFT_Jump 		UMETA(DisplayName = "Jump"),
	EFT_Land 		UMETA(DisplayName = "Land")

};