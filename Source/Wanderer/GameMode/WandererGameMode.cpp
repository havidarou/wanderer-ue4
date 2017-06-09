// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Wanderer.h"
#include "GameMode/WandererGameMode.h"
#include "GameMode/WandererHUD.h"
#include "GameMode/WandererGameInstance.h"
#include "Characters/WandererCharacter.h"

AWandererGameMode::AWandererGameMode()
{
	// set default pawn class to our character
	DefaultPawnClass = AWandererCharacter::StaticClass();	
	HUDClass = AWandererHUD::StaticClass();
}

// Before the defaultpawnclass is spawned, this method gets called, and chooses where to spawn the pawn

AActor* AWandererGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	UWandererGameInstance* WandererGameInstance = Cast<UWandererGameInstance>(GetGameInstance());

	if (WandererGameInstance->PlaceToEnter == 1) // We need the left startpoint
	{
		for (TObjectIterator<APlayerStart> Itr; Itr; ++Itr)
		{
			// Filter out objects not contained in the target world.
			if (Itr->GetWorld() != this->GetWorld())
			{
				continue;
			}
			// Do stuff
			if (Itr->PlayerStartTag == "LeftEntry")
			{
				return Cast<AActor>(*Itr);
			}
		}
	}
	if (WandererGameInstance->PlaceToEnter == 2) // We need the right start point
	{
		for (TObjectIterator<APlayerStart> Itr; Itr; ++Itr)
		{
			// Filter out objects not contained in the target world.
			if (Itr->GetWorld() != this->GetWorld())
			{
				continue;
			}
			// Do stuff
			if (Itr->PlayerStartTag == "RightEntry")
			{
				return Cast<AActor>(*Itr);
			}
		}
	}
	return nullptr;
}
