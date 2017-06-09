// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/Enemy.h"
#include "RangedEnemy.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API ARangedEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:

	ARangedEnemy();

	virtual void Attack() override;
	
protected:
	virtual void Tick(float DeltaSeconds) override;

	virtual void LoadFlipbooks() override;

	virtual void ConfigureMovement() override;

	virtual void LoadAI() override;

	virtual void LoadSounds() override;
};
