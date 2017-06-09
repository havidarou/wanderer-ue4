// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "WandererHUD.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API AWandererHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AWandererHUD();

	class UClass* gameplayWidgetClass;
	class UUserWidget* gameplayWidget;

	class UClass* menuWidgetClass;
	class UUserWidget* menuWidget;


	void changeToMenu();
	void changeToGameplay();

	UFUNCTION(BlueprintCallable, Category = "Button Events")
	void resumePressed();

protected:

	virtual void BeginPlay() override;
	
};
