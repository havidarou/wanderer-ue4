// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "DialogueAI.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API ADialogueAI : public AAIController
{
	GENERATED_BODY()
	
	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorTreeComp;

	virtual void Tick(float Deltaseconds) override;

public:

	ADialogueAI();

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	virtual void Possess(APawn *InPawn) override;
	
	uint8 IsTalking;
	uint8 DialogueWidget;
	uint8 SelfActor;
};
