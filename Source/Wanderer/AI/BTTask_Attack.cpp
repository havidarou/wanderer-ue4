// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/BasicAI/BasicEnemyAI.h"
#include "Characters/WandererCharacter.h"
#include "Characters/Enemy.h"
#include "BTTask_Attack.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ABasicEnemyAI* BasicEnemyAI = Cast<ABasicEnemyAI>(OwnerComp.GetAIOwner());
	AWandererCharacter* RiseReference = Cast<AWandererCharacter>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BasicEnemyAI->TargetToFollow));

	AEnemy* EnemyReference = Cast<AEnemy>(BasicEnemyAI->GetPawn());

	if (EnemyReference)
	{
		if (RiseReference->GetActorLocation().X >= EnemyReference->GetActorLocation().X)
			BasicEnemyAI->SetControlRotation(FRotator(0.0f, 180.0f, 0.0f));
		else
			BasicEnemyAI->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));

		EnemyReference->Attack();

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}