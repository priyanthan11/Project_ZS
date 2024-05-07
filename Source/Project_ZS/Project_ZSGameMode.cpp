// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_ZSGameMode.h"
#include "Project_ZSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProject_ZSGameMode::AProject_ZSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
