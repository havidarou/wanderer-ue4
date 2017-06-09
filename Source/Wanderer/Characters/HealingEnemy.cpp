// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "PaperFlipbookComponent.h"
#include "Characters/Shot.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BrainComponent.h"
#include "AI/BasicAI/BasicEnemyAI.h"
#include "HealingEnemy.h"


AHealingEnemy::AHealingEnemy()
{
	LoadFlipbooks();
	ConfigureMovement();
	LoadAI();
}

void AHealingEnemy::LoadFlipbooks()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleReference(TEXT("ParticleSystem'/Game/2DSideScroller/Particles/HealingEffect.HealingEffect'"));
	ParticleSystem = ParticleReference.Object;

	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-Walk.HealingEnemy-Walk'"));
	RunningAnimation = RunningAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> AttackAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-Attack.HealingEnemy-Attack'"));
	AttackAnimation = AttackAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlankFlipbookReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/BlankFlipbook.BlankFlipbook'"));
	BlankFlipbook = BlankFlipbookReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> AttackAnimationCollisionReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/BlankFlipbook.BlankFlipbook'"));
	AttackAnimationCollision = AttackAnimationCollisionReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FallAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-Fall.HealingEnemy-Fall'"));
	FallAnimation = FallAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-Hit.HealingEnemy-Hit'"));
	HitAnimation = HitAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitHoldAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-HitHold.HealingEnemy-HitHold'"));
	HitHoldAnimation = HitHoldAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DeadAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-Dead.HealingEnemy-Dead'"));
	DeadAnimation = DeadAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FlickeringDeadAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-FlickeringDead.HealingEnemy-FlickeringDead'"));
	FlickeringDeadAnimation = FlickeringDeadAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/HealingEnemy/HealingEnemy-Idle.HealingEnemy-Idle'"));
	IdleAnimation = IdleAnimationReference.Get();
}

void AHealingEnemy::ConfigureMovement()
{
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;
}

void AHealingEnemy::LoadAI()
{
	// Set the behavior tree and the ai controller
	static ConstructorHelpers::FObjectFinderOptional<UBehaviorTree> BehaviorTreeReference(TEXT("BehaviorTree'/Game/2DSideScrollerCPP/Blueprints/AI/HealingEnemyBT.HealingEnemyBT'"));
	this->behaviorTree = BehaviorTreeReference.Get();

	this->AIControllerClass = ABasicEnemyAI::StaticClass();
}

void AHealingEnemy::Heal()
{
	if (EnemyState != EEnemyCharacterState::C_DEAD) // Only if we are not dead
	{


		EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, false);
		AEnemy* reference = Cast<AEnemy>(EnemyAI->GetBlackboardComp()->GetValue<UBlackboardKeyType_Object>(EnemyAI->TargetToFollow));
		if (reference->currentHealth < reference->Health) // If it's not completely healed
		{
			if (reference->currentHealth + 0.25f < reference->Health)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, FTransform(FVector(reference->GetActorLocation().X, reference->GetActorLocation().Y, reference->GetActorLocation().Z - 100)), true);
				float TimelineLength = AttackAnimation->GetNumFrames() / (float)AttackAnimation->GetFramesPerSecond();
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AHealingEnemy::HandleHeal, TimelineLength - 0.05f*TimelineLength, false);
				EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, true);
				EnemyState = EEnemyCharacterState::C_FIGHT;
			}
			else
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, FTransform(FVector(reference->GetActorLocation().X, reference->GetActorLocation().Y, reference->GetActorLocation().Z - 100)), true);
				float TimelineLength = AttackAnimation->GetNumFrames() / (float)AttackAnimation->GetFramesPerSecond();
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AHealingEnemy::HandleHeal, TimelineLength - 0.05f*TimelineLength, false);
				EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, true);
				EnemyState = EEnemyCharacterState::C_FIGHT;
			}
		}
	}
}

void AHealingEnemy::HandleHeal()
{
	if (EnemyState != EEnemyCharacterState::C_DEAD && EnemyState != EEnemyCharacterState::C_HIT && EnemyState != EEnemyCharacterState::C_FALL && EnemyState != EEnemyCharacterState::C_HITHOLD) // Only if we are not dead
	{
		EnemyState = EEnemyCharacterState::C_IDLE;
		EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, false);
		AEnemy* reference = Cast<AEnemy>(EnemyAI->GetBlackboardComp()->GetValue<UBlackboardKeyType_Object>(EnemyAI->TargetToFollow));

		if (reference->currentHealth < reference->Health) // If it's not completely healed
		{
			if (reference->currentHealth + 0.25f < reference->Health)
				reference->currentHealth = reference->currentHealth + 0.25f;
			else
				reference->currentHealth = reference->Health;
		}
	}
}



