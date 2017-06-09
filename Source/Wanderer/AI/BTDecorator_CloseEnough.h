// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CloseEnough.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API UBTDecorator_CloseEnough : public UBTDecorator
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = Distance)
	float AcceptableDistance;
	
	UBTDecorator_CloseEnough();
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
