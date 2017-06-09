// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h"
#include "BTService_SearchAlly.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API UBTService_SearchAlly : public UBTService
{
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly, Category = "Trace")
	float SphereRadius;

	class ABasicEnemyAI* BasicEnemyAIReference;
	class AEnemy* EnemyReference;
	bool ReferenceSet;

public:

	UBTService_SearchAlly();

	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
