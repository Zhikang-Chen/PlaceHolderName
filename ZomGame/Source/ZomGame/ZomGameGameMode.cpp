// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZomGameGameMode.h"
#include "ZomGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AZomGameGameMode::AZomGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
