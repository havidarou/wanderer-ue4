// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Characters/Enemy.h"
#include "AI/BasicAI/BasicEnemyAI.h"

ABasicEnemyAI::ABasicEnemyAI()
{
	blackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BrainComponent = behaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
}

void ABasicEnemyAI::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	AEnemy* enemyCharacter = Cast<AEnemy>(InPawn);

	if (enemyCharacter && enemyCharacter->behaviorTree)
	{
		blackboardComp->InitializeBlackboard(*enemyCharacter->behaviorTree->BlackboardAsset);

		TargetToFollow = blackboardComp->GetKeyID("TargetToFollow");
		HomeLocation = blackboardComp->GetKeyID("HomeLocation");
		TargetLocation = blackboardComp->GetKeyID("TargetLocation");
		SelfActor = blackboardComp->GetKeyID("SelfActor");
		Paused = blackboardComp->GetKeyID("Paused");
		WaitTime = blackboardComp->GetKeyID("WaitTime");

		behaviorTreeComp->StartTree(*enemyCharacter->behaviorTree);
	}
}

void ABasicEnemyAI::Tick(float Deltaseconds)
{
	Super::Tick(Deltaseconds);

}
