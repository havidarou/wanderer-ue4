// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraActor.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "Characters/WandererCharacter.h"
#include "GameMode/WandererGameInstance.h"
#include "WandererLevel.h"


AWandererLevel::AWandererLevel()
{
	WandererLevelName = NAME_None;
	Left = false;
	MarkedToLeave = false;
}

void AWandererLevel::BeginPlay()
{
	Super::BeginPlay();

	// Get Rise reference
	RiseReference = Cast<AWandererCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UWandererGameInstance* WandererGameInstance = Cast<UWandererGameInstance>(GetGameInstance());

	int MapIndex = -1;
	for (int i = 0; i < WandererGameInstance->Maps.Num(); i++)
	{
		if (WandererGameInstance->Maps[i].Name == WandererGameInstance->CurrentMapName)
		{
			MapIndex = i;
		}
	}

	// Check where do we enter from and place Rise properly in the level
	if (Cast<UWandererGameInstance>(GetGameInstance())->PlaceToEnter == 1) // We enter from the left side
	{
		RiseReference->GetCameraBoom()->bEnableCameraLag = false;
		RiseReference->SetActorLocation(WandererGameInstance->Maps[MapIndex].EntryPoints[0]);
		RiseReference->GetCameraBoom()->bEnableCameraLag = true;
	}
	if (Cast<UWandererGameInstance>(GetGameInstance())->PlaceToEnter == 2) // We enter from the right side
	{
		RiseReference->GetCameraBoom()->bEnableCameraLag = false;
		RiseReference->SetActorLocation(WandererGameInstance->Maps[MapIndex].EntryPoints[1]);
		RiseReference->GetCameraBoom()->bEnableCameraLag = true;
		RiseReference->Controller->SetControlRotation(FRotator(0.0f, 180.0f, 0.0f)); // Flip, because we enter from the right side
	}

	// Looking for the camera transition triggers
	for (TObjectIterator<UBoxComponent> Itr; Itr; ++Itr)
	{
		// Filter out objects not contained in the target world.
		if (Itr->GetWorld() != this->GetWorld())
		{
			continue;
		}
		// Do stuff
		if (Itr->GetFullName().Contains("LeftTriggerBox"))
		{
			LeftTriggerBox = Cast<UBoxComponent>(*Itr);
		}

		if (Itr->GetFullName().Contains("RightTriggerBox"))
		{
			RightTriggerBox = Cast<UBoxComponent>(*Itr);
		}
	}

	// Looking for the map transition triggers
	for (TObjectIterator<UBoxComponent> Itr; Itr; ++Itr)
	{
		// Filter out objects not contained in the target world.
		if (Itr->GetWorld() != this->GetWorld())
		{
			continue;
		}
		// Do stuff
		if (Itr->GetFullName().Contains("LeftTransition"))
		{
			LeftTransition = Cast<UBoxComponent>(*Itr);
		}

		if (Itr->GetFullName().Contains("RightTransition"))
		{
			RightTransition = Cast<UBoxComponent>(*Itr);
		}
	}

	// Looking for the fixed cameras
	for (TObjectIterator<ACameraActor> Itr; Itr; ++Itr)
	{
		// Filter out objects not contained in the target world.
		if (Itr->GetWorld() != this->GetWorld())
		{
			continue;
		}
		// Do stuff
		if (Itr->GetFullName().Contains("LeftFixedCamera"))
		{
			LeftFixedCamera = Cast<ACameraActor>(*Itr);
		}

		if (Itr->GetFullName().Contains("RightFixedCamera"))
		{
			RightFixedCamera = Cast<ACameraActor>(*Itr);
		}
	}

	// Finding the LevelSequenceActor

	for (TObjectIterator<ALevelSequenceActor> Itr; Itr; ++Itr)
	{
		// Filter out objects not contained in the target world.
		if (Itr->GetWorld() != this->GetWorld())
		{
			continue;
		}
		else // There's just one in our world
		{
			LevelSequenceActor = Cast<ALevelSequenceActor>(*Itr);
		}
	}


	// We load the fade in level sequence and play it at the beginning
	FStringAssetReference LevelSequence2("LevelSequence'/Game/2DSideScrollerCPP/Blueprints/LevelSequences/MainMenuSequence.MainMenuSequence'");
	LevelSequenceActor->SetSequence(Cast<ULevelSequence>(LevelSequence2.TryLoad()));
//	LevelSequenceActor->SequencePlayer->Play();


	
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1.0f, 1.0f, 1.0f, FLinearColor::Black);

	RiseReference->DisableInput(GetWorld()->GetFirstPlayerController());

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererLevel::HandleFadeIn, 0.75f, false);
}

void AWandererLevel::Pollo_Event_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "pollicopollico");
	RiseReference->CharacterState = ECharacterState::C_INTRO;
	RiseReference->SetActorLocation(FVector(RiseReference->GetActorLocation().X + 1, RiseReference->GetActorLocation().Y + 1, RiseReference->GetActorLocation().Z + 1));
}

void AWandererLevel::Tick(float Deltaseconds)
{
	if (MarkedToLeave == false)
	{
	// Fixed camera transitions check

	// Right one
	if (RiseReference->GetActorLocation().X > RightTriggerBox->GetComponentLocation().X)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(RightFixedCamera, 0.15f);
		Left = false;
	}

	// We are in the middle of the map
	if (RiseReference->GetActorLocation().X < RightTriggerBox->GetComponentLocation().X && RiseReference->GetActorLocation().X > LeftTriggerBox->GetComponentLocation().X)
	{
		if (Left)
		{
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(RiseReference, 0.25f);
		}
		else
		{
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(RiseReference, 0.25f);
		}
	}

	// Left one
	if (RiseReference->GetActorLocation().X < LeftTriggerBox->GetComponentLocation().X)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(LeftFixedCamera, 0.15f);
		Left = true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Map out transitions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (RiseReference->GetActorLocation().X < LeftTransition->GetComponentLocation().X) // Transition to the leftish map
	{
		UWandererGameInstance* WandererGameInstance = Cast<UWandererGameInstance>(GetGameInstance());

		WandererGameInstance->PlaceToEnter = 2; // We leave to the left, so we enter to the right

		// We search for the map located to the left of the current one
		for (int i = 0; i < WandererGameInstance->Maps.Num(); i++)
		{
			if (WandererGameInstance->Maps[i].Name == WandererGameInstance->CurrentMapName)
			{
				NewMapName = WandererGameInstance->Maps[i].AdyacentMaps[0];
			}
		}

		// We update the current map
		if (NewMapName != "No")
		{
			WandererGameInstance->CurrentMapName = NewMapName;

			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 1.0f, 0.5f, FLinearColor::Black);
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererLevel::HandleFadeOut, 0.5f, false);

			MarkedToLeave = true;
		}

	}

	if (RiseReference->GetActorLocation().X > RightTransition->GetComponentLocation().X) // Transition to the rightish map
	{
		UWandererGameInstance* WandererGameInstance = Cast<UWandererGameInstance>(GetGameInstance());

		WandererGameInstance->PlaceToEnter = 1; // We leave to the right, so we enter to the left

		// We search for the map located to the right of the current one
		for (int i = 0; i < WandererGameInstance->Maps.Num(); i++)
		{
			if (WandererGameInstance->Maps[i].Name == WandererGameInstance->CurrentMapName)
			{
				NewMapName = WandererGameInstance->Maps[i].AdyacentMaps[1];
			}
		}


		if (NewMapName != "No")
		{
			// We update the current map
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, NewMapName);
			WandererGameInstance->CurrentMapName = NewMapName;

			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 1.0f, 0.5f, FLinearColor::Black);
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererLevel::HandleFadeOut, 0.25f, false);
			MarkedToLeave = true;
		}

	}
	}
}

void AWandererLevel::HandleFadeOut()
{
	// And then open it
	UWandererGameInstance* WandererGameInstance = Cast<UWandererGameInstance>(GetGameInstance());
	UGameplayStatics::OpenLevel((UObject*)WandererGameInstance, FName(*NewMapName));

}

void AWandererLevel::HandleFadeIn()
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1.0f, 0.0f, 3.0f, FLinearColor::Black);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererLevel::HandleFadeIn2, 1.0f, false);
}

void AWandererLevel::HandleFadeIn2()
{
	RiseReference->EnableInput(GetWorld()->GetFirstPlayerController());
	// Spawn enemies!
}