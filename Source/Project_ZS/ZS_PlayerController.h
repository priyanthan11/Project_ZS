// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ZS_API AZS_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AZS_PlayerController();

	/** PlayerCamera class should be set for each game, otherwise Engine.PlayerCameraManager is used */
	virtual void OnPossess(APawn* aPawn) override;
	
};
