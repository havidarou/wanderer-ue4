// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/BasicAI/BasicEnemyAI.h"
#include "Characters/WandererCharacter.h"
#include "Characters/HealingEnemy.h"
#include "BTTask_Heal.h"

UBTTask_Heal::UBTTask_Heal()
{
	NodeName = "Heal";
}

EBTNodeResult::Type UBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ABasicEnemyAI* BasicEnemyAI = Cast<ABasicEnemyAI>(OwnerComp.GetAIOwner());

	AHealingEnemy* EnemyReference = Cast<AHealingEnemy>(BasicEnemyAI->GetPawn());

	if (EnemyReference)
	{
		EnemyReference->Heal();

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}



