// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/Enemy.h"
#include "HealingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API AHealingEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:

	AHealingEnemy();

	void Heal();

protected:
	virtual void LoadFlipbooks() override;

	virtual void ConfigureMovement() override;

	virtual void LoadAI() override;

	UFUNCTION()
		void HandleHeal();
};
