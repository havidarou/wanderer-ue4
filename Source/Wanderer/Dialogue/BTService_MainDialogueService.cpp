// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Dialogue/DialogueAI.h"
#include "Characters/DialogueNPC.h"
#include "Characters/WandererCharacter.h"
#include "Components/WidgetComponent.h"
#include "Dialogue/BTService_MainDialogueService.h"

UBTService_MainDialogueService::UBTService_MainDialogueService()
{
	/** We need to be able to create instances of this node in order to use it in differente behavior trees */
	bCreateNodeInstance = true;
	bNotifyBecomeRelevant = true;
	radius = 200.0f;

	// Loading the dialogue widget
	static ConstructorHelpers::FClassFinder<UUserWidget> DialogueWidgetObj(TEXT("/Game/2DSideScrollerCPP/Blueprints/Dialogue/DialogueWidget.DialogueWidget_C"));
	if (DialogueWidgetObj.Succeeded()) {
		DialogueWidgetClass = DialogueWidgetObj.Class;
	}
	else {
		// dialogueWidgetObj not found
		DialogueWidgetClass = nullptr;
	}
	
	WidgetCreated = false;
}

void UBTService_MainDialogueService::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	/** We get the AI controller */
	ADialogueAI* DialogueAI = Cast<ADialogueAI>(OwnerComp.GetAIOwner());

	/** We get the character */
	ADialogueNPC* DialogueNPC = Cast<ADialogueNPC>(DialogueAI->GetPawn());
	
	DialogueNPCReference = DialogueNPC;


}

void UBTService_MainDialogueService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	/** We need to create the widget here because if we try to do it before, the playercontroller is null, so...this is a bit sloppy */
	if (DialogueWidgetClass && GetWorld()->GetFirstPlayerController() != nullptr && WidgetCreated == false) {

		DialogueWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), this->DialogueWidgetClass);
		ADialogueAI* DialogueAI = Cast<ADialogueAI>(OwnerComp.GetAIOwner());
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(DialogueAI->DialogueWidget, DialogueWidget);
		WidgetCreated = true;
	}

	RiseReference = Cast<AWandererCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	/** If Rise is close enough */
	if (RiseReference->GetDistanceTo(DialogueNPCReference) < radius)
	{
		/** If we are already talking, then we hide the talk icon */
		if (RiseReference->IsTalking)
			DialogueNPCReference->TalkWidgetComp->SetVisibility(false);
		/** But if we are not talking, then we display the talk icon */
		else DialogueNPCReference->TalkWidgetComp->SetVisibility(true);

		RiseReference->CanTalk = true;

		if (RiseReference->IsTalking)
		{

			ADialogueAI* DialogueAI = Cast<ADialogueAI>(OwnerComp.GetAIOwner());
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(DialogueAI->IsTalking, RiseReference->IsTalking);
		}
	}
	else /** Rise is not close enough, so we don't display the talk icon, and of course, he cannot talk to us */
	{
		DialogueNPCReference->TalkWidgetComp->SetVisibility(false);
		RiseReference->CanTalk = false;
	}
}


