// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperCharacter.h"
#include "DialogueNPC.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API ADialogueNPC : public APaperCharacter
{
	GENERATED_BODY()
	
	virtual void Tick(float DeltaSeconds) override;
	
public:

	ADialogueNPC();

	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Talk Widget")
	class UWidgetComponent* TalkWidgetComp;

	class UClass* TalkWidgetClass;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float currentHealth;

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	void UpdateCharacter();
};
