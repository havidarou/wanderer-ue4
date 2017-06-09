// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Characters/DialogueNPC.h"
#include "Dialogue/DialogueAI.h"

ADialogueAI::ADialogueAI()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
}

void ADialogueAI::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ADialogueNPC* DialogueCharacter = Cast<ADialogueNPC>(InPawn);

	if (DialogueCharacter && DialogueCharacter->BehaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*DialogueCharacter->BehaviorTree->BlackboardAsset);

		IsTalking = BlackboardComp->GetKeyID("IsTalking");
		DialogueWidget = BlackboardComp->GetKeyID("DialogueWidget");
		SelfActor = BlackboardComp->GetKeyID("SelfActor");

		BehaviorTreeComp->StartTree(*DialogueCharacter->BehaviorTree);
	}
}

void ADialogueAI::Tick(float Deltaseconds)
{
	Super::Tick(Deltaseconds);
}
