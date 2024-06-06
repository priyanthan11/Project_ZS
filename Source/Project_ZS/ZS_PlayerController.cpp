// Fill out your copyright notice in the Description page of Project Settings.


#include "ZS_PlayerController.h"
#include "ZS_PlayerCameraManager.h"

AZS_PlayerController::AZS_PlayerController()
{

}

void AZS_PlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, TEXT("OnPosses Called IN PC"));
	if (aPawn != nullptr)
	{
		Cast<AZS_PlayerCameraManager>(PlayerCameraManager)->OnPosses(aPawn);

	}

}

