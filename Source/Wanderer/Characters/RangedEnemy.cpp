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
#include "RangedEnemy.h"

ARangedEnemy::ARangedEnemy()
{
	LoadFlipbooks();
	ConfigureMovement();
	LoadAI();
	LoadSounds();
}

void ARangedEnemy::LoadSounds()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> FootstepSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-FootstepSound.CloseEnemy-FootstepSound'"));
	FootstepSound = FootstepSoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> AttackedSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/RangedEnemy/RangedEnemy-AttackedSound.RangedEnemy-AttackedSound'"));
	AttackedSound = AttackedSoundReference.Object;

	static ConstructorHelpers::FObjectFinderOptional<USoundCue> AttackSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/RangedEnemy/RangedEnemy-ShootSound.RangedEnemy-ShootSound'"));
	AttackSound = AttackSoundReference.Get();

	static ConstructorHelpers::FObjectFinder<USoundCue> IdleSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-FootstepSound.CloseEnemy-FootstepSound'"));
	IdleSound = IdleSoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> FallSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-FootstepSound.CloseEnemy-FootstepSound'"));
	FallSound = FallSoundReference.Object;
}

void ARangedEnemy::LoadFlipbooks()
{
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-Walk.RangedEnemy-Walk'"));
	RunningAnimation = RunningAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> AttackAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-Attack.RangedEnemy-Attack'"));
	AttackAnimation = AttackAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlankFlipbookReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/BlankFlipbook.BlankFlipbook'"));
	BlankFlipbook = BlankFlipbookReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> AttackAnimationCollisionReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/BlankFlipbook.BlankFlipbook'"));
	AttackAnimationCollision = AttackAnimationCollisionReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FallAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-Fall.RangedEnemy-Fall'"));
	FallAnimation = FallAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-Hit.RangedEnemy-Hit'"));
	HitAnimation = HitAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitHoldAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-HitHold.RangedEnemy-HitHold'"));
	HitHoldAnimation = HitHoldAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DeadAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-Dead.RangedEnemy-Dead'"));
	DeadAnimation = DeadAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FlickeringDeadAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-FlickeringDead.RangedEnemy-FlickeringDead'"));
	FlickeringDeadAnimation = FlickeringDeadAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/RangedEnemy/RangedEnemy-Idle.RangedEnemy-Idle'"));
	IdleAnimation = IdleAnimationReference.Get();
}

void ARangedEnemy::ConfigureMovement()
{
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;
}

void ARangedEnemy::LoadAI()
{
	// Set the behavior tree and the ai controller
	static ConstructorHelpers::FObjectFinderOptional<UBehaviorTree> BehaviorTreeReference(TEXT("BehaviorTree'/Game/2DSideScrollerCPP/Blueprints/AI/RangedEnemyBT.RangedEnemyBT'"));
	this->behaviorTree = BehaviorTreeReference.Get();

	this->AIControllerClass = ABasicEnemyAI::StaticClass();
}

void ARangedEnemy::Tick(float DeltaSeconds)
{

	Super::Tick(DeltaSeconds);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(FacingRight));

}

void ARangedEnemy::Attack()
{
	if (EnemyState != EEnemyCharacterState::C_DEAD) // Only if we are not dead
	{
		AudioComponent->SetSound(AttackSound);
		AudioComponent->Play();

		float TimelineLength = AttackAnimation->GetNumFrames() / (float)AttackAnimation->GetFramesPerSecond();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::HandleAttack, TimelineLength - 0.05f*TimelineLength, false);

		EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, true);
		EnemyState = EEnemyCharacterState::C_FIGHT;



		FVector Location = this->GetActorLocation();
		if (FacingRight)
			Location = Location + FVector(75.0f, 0.0f, 30.0f);
		else
			Location = Location + FVector(-75.0f, 0.0f, 30.0f);
		FActorSpawnParameters SpawnInfo;
		GetWorld()->SpawnActor<AShot>(Location, this->GetActorRotation(), SpawnInfo);
	}
}
