// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CloseDialogueNPC.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API UBTTask_CloseDialogueNPC : public UBTTaskNode
{
	GENERATED_BODY()

public:

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
