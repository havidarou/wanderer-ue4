// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "GameMode/WandererHUD.h"
#include "Components/Button.h"

AWandererHUD::AWandererHUD()
{
	// Loading the gameplay HUD widget
	static ConstructorHelpers::FClassFinder<UUserWidget> gameplayWidgetObj(TEXT("/Game/2DSideScrollerCPP/Blueprints/HUD/GameplayHUD.GameplayHUD_C"));
	if (gameplayWidgetObj.Succeeded()) {
		gameplayWidgetClass = gameplayWidgetObj.Class;
	}
	else {
		// gameplayWidgetObj not found
		gameplayWidgetClass = nullptr;
	}

	// Loading the menu HUD widget
	static ConstructorHelpers::FClassFinder<UUserWidget> menuWidgetObj(TEXT("/Game/2DSideScrollerCPP/Blueprints/HUD/MenuHUD.MenuHUD_C"));
	if (menuWidgetObj.Succeeded()) {
		menuWidgetClass = menuWidgetObj.Class;
	}
	else {
		// menuWidgetObj not found
		menuWidgetClass = nullptr;
	}
}

void AWandererHUD::BeginPlay()
{
	Super::BeginPlay();

	if (gameplayWidgetClass) {
		// the player controller should be constructed by now so we can get a reference to it
		gameplayWidget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), this->gameplayWidgetClass);
		gameplayWidget->AddToViewport();
	}

	if (menuWidgetClass) {
		// the player controller should be constructed by now so we can get a reference to it
		menuWidget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), this->menuWidgetClass);
	}
}

void AWandererHUD::resumePressed() {

	menuWidget->RemoveFromViewport();
	gameplayWidget->AddToViewport();

	APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != NULL)
	{
		MyPlayer->SetPause(false);
		FInputModeGameOnly gameMode;
		MyPlayer->SetInputMode(gameMode);
	}
}

void AWandererHUD::changeToMenu()
{
	gameplayWidget->RemoveFromViewport();
	menuWidget->AddToViewport();

	APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != NULL)
	{
		MyPlayer->SetPause(true);
		FInputModeUIOnly uiMode;
		MyPlayer->SetInputMode(uiMode);
		UButton* resumeButton = Cast<UButton>(menuWidget->GetWidgetFromName(FName(TEXT("ResumeButton"))));
		resumeButton->SetKeyboardFocus();
	}
}


