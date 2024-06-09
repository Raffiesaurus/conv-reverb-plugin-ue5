// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReverbPluginGameMode.h"
#include "ReverbPluginCharacter.h"
#include "UObject/ConstructorHelpers.h"

AReverbPluginGameMode::AReverbPluginGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
