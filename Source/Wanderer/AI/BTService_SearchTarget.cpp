// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/BasicAI/BasicEnemyAI.h"
#include "Characters/Enemy.h"
#include "Characters/WandererCharacter.h"
#include "BTService_SearchTarget.h"

UBTService_SearchTarget::UBTService_SearchTarget()
{
	SphereRadius = 3000.0f;
	ReferenceSet = false;
}

void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{


	/** We do this check in the tick node because we are not sure when all of the initialization process will occur, and then protect the recasting with a bool, sloppy, but useful */
	if (ReferenceSet == false)
	{
		BasicEnemyAIReference = Cast<ABasicEnemyAI>(OwnerComp.GetAIOwner());
		EnemyReference = Cast<AEnemy>(BasicEnemyAIReference->GetPawn());
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BasicEnemyAIReference->HomeLocation, EnemyReference->GetActorLocation());

		ReferenceSet = true;
	}
	// We want to ignore in the trace all enemy objects 
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), ActorsToIgnore);

	// The trace will start at current enemy location 
	FVector MyLocation = EnemyReference->GetActorLocation();
	
	TArray<FHitResult> HitResults;

	// Shape of the collision object we are sweeping 
	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(SphereRadius);

	// And we are looking for pawn type objects, in this case, Rise 
	FCollisionObjectQueryParams DesiredObjectTypes;
	DesiredObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	
	// Adding the actors to ignore 
	FCollisionQueryParams TraceParams(FName("Trace"), false, ActorsToIgnore[0]);
	TraceParams.AddIgnoredActors(ActorsToIgnore);

	FHitResult LineHitResult;
	// Sweep the sphere ignoring enemy alike actors and looking for the pawn 
	if (GetWorld()->SweepMultiByObjectType(HitResults, MyLocation, MyLocation + FVector(0.0f, 0.0f, 15.0f), FQuat(), DesiredObjectTypes, CollisionShape, TraceParams))
	{
		bool CorrespondenceFound = false;
		// If there were hits, we cycle them 
		for (auto It = HitResults.CreateIterator(); It && CorrespondenceFound == false; It++)
		{

			// If some of these hits lead us to the desired target 
			if (GetWorld()->LineTraceSingleByChannel(LineHitResult, MyLocation, HitResults[It.GetIndex()].GetActor()->GetActorLocation(), ECollisionChannel::ECC_Pawn, TraceParams))
			{
				if (LineHitResult.GetActor() == HitResults[It.GetIndex()].GetActor())
				{
					OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(BasicEnemyAIReference->TargetToFollow, LineHitResult.GetActor());
					OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BasicEnemyAIReference->TargetLocation, LineHitResult.GetActor()->GetActorLocation());
					CorrespondenceFound = true;
				}
				else
				{
					//OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(BasicEnemyAIReference->TargetToFollow, nullptr);
				}
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("No hay hit"));
			}
		}
	}
}