// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h"
#include "BTService_MainDialogueService.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API UBTService_MainDialogueService : public UBTService
{
	GENERATED_BODY()

	bool WidgetCreated;
public:

	class AWandererCharacter* RiseReference;
	class ADialogueNPC* DialogueNPCReference;

	UPROPERTY(EditInstanceOnly, Category = Dialogue)
	float radius;

	class UClass* DialogueWidgetClass;
	class UUserWidget* DialogueWidget;

	UBTService_MainDialogueService();

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
