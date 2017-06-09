// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "WandererLevel.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API AWandererLevel : public ALevelScriptActor
{
	GENERATED_BODY()

	bool Left;
	bool MarkedToLeave;
	FString NewMapName;

	UFUNCTION()
	void HandleFadeOut();

	UFUNCTION()
	void HandleFadeIn();
	void HandleFadeIn2();
	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	class ALevelSequenceActor* LevelSequenceActor;

	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	FName WandererLevelName;
	
	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	class UBoxComponent* LeftTriggerBox;

	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	class UBoxComponent* RightTransition;

	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	class UBoxComponent* LeftTransition;

	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	class UBoxComponent* RightTriggerBox;

	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	class ACameraActor* LeftFixedCamera;

	UPROPERTY(EditDefaultsOnly, Category = WandererLevel)
	class ACameraActor* RightFixedCamera;

	class AWandererCharacter* RiseReference;

public:

	AWandererLevel();

	UFUNCTION(BlueprintNativeEvent)
		void Pollo_Event();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
