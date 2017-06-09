// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BasicEnemyAI.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API ABasicEnemyAI : public AAIController
{
	GENERATED_BODY()
	
	UPROPERTY(transient)
	class UBlackboardComponent* blackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* behaviorTreeComp;
	
	virtual void Tick(float Deltaseconds) override;

public:

	ABasicEnemyAI();

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return blackboardComp; }
	virtual void Possess(APawn *InPawn) override;
	
	uint8 TargetToFollow;
	uint8 HomeLocation;
	uint8 TargetLocation;
	uint8 SelfActor;
	uint8 Paused;
	uint8 WaitTime;
};
