// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Characters/DialogueNPC.h"
#include "Dialogue/DialogueAI.h"
#include "Characters/WandererCharacter.h"
#include "Dialogue/BTTask_CloseDialogueNPC.h"


EBTNodeResult::Type UBTTask_CloseDialogueNPC::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADialogueAI* DialogueAI = Cast<ADialogueAI>(OwnerComp.GetAIOwner());
	AWandererCharacter* rise = Cast<AWandererCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	rise->IsTalking = false;

	// We are closing the dialogue, so we set IsTalking to false
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(DialogueAI->IsTalking, false);

	return EBTNodeResult::Succeeded;
}

