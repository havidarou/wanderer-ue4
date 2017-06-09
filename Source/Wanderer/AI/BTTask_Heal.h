// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Heal.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API UBTTask_Heal : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_Heal();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
