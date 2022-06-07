// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_II_P2GameMode.h"
#include "GE_II_P2HUD.h"
#include "GE_II_P2Character.h"
#include "UObject/ConstructorHelpers.h"

AGE_II_P2GameMode::AGE_II_P2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGE_II_P2HUD::StaticClass();
}
