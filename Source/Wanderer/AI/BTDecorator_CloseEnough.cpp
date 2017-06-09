// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/BasicAI/BasicEnemyAI.h"
#include "Characters/WandererCharacter.h"
#include "BTDecorator_CloseEnough.h"

UBTDecorator_CloseEnough::UBTDecorator_CloseEnough()
{
	AcceptableDistance = 75.0f;
}

bool UBTDecorator_CloseEnough::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ABasicEnemyAI* BasicEnemyAI = Cast<ABasicEnemyAI>(OwnerComp.GetAIOwner());

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BasicEnemyAI->TargetToFollow));

	if (FVector(Target->GetActorLocation() - BasicEnemyAI->GetPawn()->GetActorLocation()).Size() >= AcceptableDistance)
	{
		return true;
	}
	else {
		return false;
	}
}


