// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "PaperFlipbookComponent.h"
#include "AI/BasicAI/BasicEnemyAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BrainComponent.h"
#include "WandererCharacter.h"
#include "Components/WidgetComponent.h"
#include "Runtime/UMG/Public/Components/ProgressBar.h"
#include "Characters/Enemy.h"

AEnemy::AEnemy()
{
	// Stats and others initialization
	Health = 1.0f;
	currentHealth = Health;
	MarkedToDestroy = false;
	EnemyState = EEnemyCharacterState::C_IDLE;

	// Create audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);

	// Create the weapon flipbook and attach it to the root
	Weapon = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(RootComponent);
	Weapon->SetIsReplicated(true);
	Weapon->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));

	// Configure character movement
	ConfigureMovement();

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	GetSprite()->CastShadow = 1;

	// Relative locations and scale of the visual components
	GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	GetSprite()->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	Weapon->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	
	// Dynamic binding of the event on begin overlap
	Weapon->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::InflictDamage);

	bReplicates = true;

	// Hide the weapon in game, we just need it to fire collision
	Weapon->SetHiddenInGame(true);

	// Health Widget Construction

	// Loading the talk widget
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthWidgetObj(TEXT("/Game/2DSideScrollerCPP/Blueprints/EnemyHealthBar.EnemyHealthBar_C"));
	if (HealthWidgetObj.Succeeded()) {
		HealthWidgetClass = HealthWidgetObj.Class;
	}
	else {
		// TalkWidgetObj not found
		HealthWidgetClass = nullptr;
	}

	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Widget"));
	HealthWidgetComp->SetupAttachment(RootComponent);
	HealthWidgetComp->SetVisibility(true);
	HealthWidgetComp->SetWidgetClass(HealthWidgetClass);
	HealthWidgetComp->SetWorldScale3D(FVector(0.5f));
	HealthWidgetComp->SetRelativeLocation(FVector(100.0f, 0.0f, -230.0f));
	HealthWidgetComp->SetAbsolute(false, true, false);
	HealthWidgetComp->SetRelativeRotation(FQuat(FRotator(0.0f, 90.0f, 0.0f)));

	// Collision Presets
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapOnlyPawnAndDynamic"));
	GetSprite()->SetCollisionProfileName(TEXT("CharacterMesh"));
	Weapon->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	LoadFlipbooks();

	// Set the main flipbook material
	UMaterial* material = LoadObject<UMaterial>(NULL, TEXT("Material'/Paper2D/DefaultLitSpriteMaterial.DefaultLitSpriteMaterial'"));
	GetSprite()->SetMaterial(0, material);

	// Set the behavior tree and the ai controller
	LoadAI();

	LoadSounds();
}

void AEnemy::LoadSounds()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> FootstepSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-FootstepSound.CloseEnemy-FootstepSound'"));
	FootstepSound = FootstepSoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> AttackedSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-AttackedSound.CloseEnemy-AttackedSound'"));
	AttackedSound = AttackedSoundReference.Object;

	static ConstructorHelpers::FObjectFinderOptional<USoundCue> AttackSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-AttackSound.CloseEnemy-AttackSound'"));
	AttackSound = AttackSoundReference.Get();

	static ConstructorHelpers::FObjectFinder<USoundCue> IdleSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-FootstepSound.CloseEnemy-FootstepSound'"));
	IdleSound = IdleSoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> FallSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/CloseEnemy/CloseEnemy-FootstepSound.CloseEnemy-FootstepSound'"));
	FallSound = FallSoundReference.Object;
}

void AEnemy::LoadAI()
{
	static ConstructorHelpers::FObjectFinderOptional<UBehaviorTree> BehaviorTreeReference(TEXT("BehaviorTree'/Game/2DSideScrollerCPP/Blueprints/AI/BasicEnemyBT.BasicEnemyBT'"));
	this->behaviorTree = BehaviorTreeReference.Get();

	this->AIControllerClass = ABasicEnemyAI::StaticClass();
}

void AEnemy::ConfigureMovement()
{
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;
}

void AEnemy::LoadFlipbooks()
{
	// Loading specific animations

	// For the animation component itself
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-Walk.CloseEnemy-Walk'"));
	RunningAnimation = RunningAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> AttackAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-Attack.CloseEnemy-Attack'"));
	AttackAnimation = AttackAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlankFlipbookReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/BlankFlipbook.BlankFlipbook'"));
	BlankFlipbook = BlankFlipbookReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> AttackAnimationCollisionReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-AttackCollision.CloseEnemy-AttackCollision'"));
	AttackAnimationCollision = AttackAnimationCollisionReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FallAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-Fall.CloseEnemy-Fall'"));
	FallAnimation = FallAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-Hit.CloseEnemy-Hit'"));
	HitAnimation = HitAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitHoldAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-HitHold.CloseEnemy-HitHold'"));
	HitHoldAnimation = HitHoldAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DeadAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-Dead.CloseEnemy-Dead'"));
	DeadAnimation = DeadAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FlickeringDeadAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-FlickeringDead.CloseEnemy-FlickeringDead'"));
	FlickeringDeadAnimation = FlickeringDeadAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/CloseEnemy/CloseEnemy-Idle.CloseEnemy-Idle'"));
	IdleAnimation = IdleAnimationReference.Get();
}

void AEnemy::HandleHit()
{
	if (currentHealth >= 0.0f && EnemyState != EEnemyCharacterState::C_FALL && EnemyState != EEnemyCharacterState::C_DEAD)
	{
		EnemyState = EEnemyCharacterState::C_IDLE;
		EnemyAI->GetBrainComponent()->RestartLogic();
		EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, false);
	}
}

void AEnemy::HandleDestroy()
{
	Destroy();
}

void AEnemy::HandleDead()
{
	if (EnemyState != EEnemyCharacterState::C_DEAD) // Only if we are not dead
	{
		EnemyState = EEnemyCharacterState::C_DEAD;
		EnemyAI->GetBrainComponent()->StopLogic("Dead");
		GetController()->UnPossess();
		MarkedToDestroy = true;
		float TimelineLength = FlickeringDeadAnimation->GetNumFrames() / (float)FlickeringDeadAnimation->GetFramesPerSecond();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::HandleDestroy, TimelineLength - 0.05f*TimelineLength, false);
	}
}

void AEnemy::HandleAttack()
{
	if (EnemyState != EEnemyCharacterState::C_DEAD && EnemyState != EEnemyCharacterState::C_HIT && EnemyState != EEnemyCharacterState::C_FALL && EnemyState != EEnemyCharacterState::C_HITHOLD) // Only if we are not dead
	{
		EnemyState = EEnemyCharacterState::C_IDLE;
		EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, false);
	}
}

void AEnemy::Attack()
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
	}
}

void AEnemy::UpdateSound()
{
	switch (EnemyState)
	{
	case EEnemyCharacterState::C_RUN:

			if (GetSprite()->GetPlaybackPositionInFrames() == 0)
			{
				AudioComponent->SetSound(FootstepSound);
				AudioComponent->Play();

			}
			if (GetSprite()->GetPlaybackPositionInFrames() == 5)
			{
				AudioComponent->SetSound(FootstepSound);
				AudioComponent->Play();
			}

		break;

	}
}

void AEnemy::InflictDamage(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AWandererCharacter>(OtherActor) && OtherComp->GetName() == "Sprite0") // If Rise is hit
	{

		if (Cast<AWandererCharacter>(OtherActor)->CharacterState != ECharacterState::C_BLOCK && Cast<AWandererCharacter>(OtherActor)->CharacterState != ECharacterState::C_BLOCKHOLD) // If we are not blocking
		{
			Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.15f, false, false);
		}
		else // We are blocking
		{
			if (Cast<AWandererCharacter>(OtherActor)->IsFacingRight() == true && this->GetActorLocation().X > OtherActor->GetActorLocation().X)
			{
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCK) // Precise block
				{
					// Play fancy animation
					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
				}
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCKHOLD) // Non precise block
				{
					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
				}
			} 
			else if (Cast<AWandererCharacter>(OtherActor)->IsFacingRight() == false && this->GetActorLocation().X < OtherActor->GetActorLocation().X)
			{
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCK) // Precise block
				{
					// Play fancy animation
					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
				}
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCKHOLD) // Non precise block
				{
					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
				}
			}
			else // Blocking, but wrong direction
			{
				Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.15f, false, false);
			}
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "No es Enemigo");
	}
}

void AEnemy::HandleHitWithAir()
{
	if (currentHealth >= 0.0f && EnemyState != EEnemyCharacterState::C_FALL && EnemyState != EEnemyCharacterState::C_DEAD)
	{
		AEnemy::StopJumping();
		EnemyState = EEnemyCharacterState::C_HITHOLD;
	}
}

void AEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	EnemyState = EEnemyCharacterState::C_IDLE;
	EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, false);
	//EnemyAI->GetBrainComponent()->RestartLogic();
}

void AEnemy::ReceiveDamage(float DamageIn, bool AirThrow, bool Fall, bool ThrowDirection)
{
	if (EnemyState != EEnemyCharacterState::C_DEAD) // Only if we are not dead
	{
		AudioComponent->SetSound(AttackedSound);
		AudioComponent->Play();
		if (AirThrow)
		{
			AEnemy::Jump();

			float TimelineLength = HitAnimation->GetNumFrames() / (float)HitAnimation->GetFramesPerSecond();
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::HandleHitWithAir, TimelineLength - 0.05f*TimelineLength, false);

			EnemyState = EEnemyCharacterState::C_HIT;
			currentHealth = currentHealth - DamageIn;

			EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Bool>(EnemyAI->Paused, true);

			//EnemyAI->GetBrainComponent()->StopLogic("HITAIR");
		}
		else
		{
			float TimelineLength = HitAnimation->GetNumFrames() / (float)HitAnimation->GetFramesPerSecond();
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::HandleHit, TimelineLength - 0.05f*TimelineLength, false);

				/*
			if (ThrowDirection)
				this->GetCharacterMovement()->AddForce(FVector(-7000000.0f, 0.0f, 0.0f));
			else
				this->GetCharacterMovement()->AddForce(FVector(7000000.0f, 0.0f, 0.0f));
				*/
			EnemyState = EEnemyCharacterState::C_HIT;
			currentHealth = currentHealth - DamageIn;	
			EnemyAI->GetBrainComponent()->StopLogic("HIT");
		}
	}
}

void AEnemy::UpdateAnimation()
{
	UPaperFlipbook* DesiredAnimation = nullptr;

	switch (EnemyState) {
	case EEnemyCharacterState::C_IDLE:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		DesiredAnimation = IdleAnimation;
		break;
	case EEnemyCharacterState::C_RUN:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		DesiredAnimation = RunningAnimation;
		break;
	case EEnemyCharacterState::C_FIGHT:
		Weapon->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		GetSprite()->SetRelativeLocation(FVector(00.0f, 0.0f, -100.0f));
		DesiredAnimation = AttackAnimation;
		Weapon->SetFlipbook(AttackAnimationCollision);
		break;
	case EEnemyCharacterState::C_HIT:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		DesiredAnimation = HitAnimation;
		break;
	case EEnemyCharacterState::C_HITHOLD:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		DesiredAnimation = HitHoldAnimation;
		break;
	case EEnemyCharacterState::C_TALK:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		DesiredAnimation = IdleAnimation;
		break;
	case EEnemyCharacterState::C_FALL:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		DesiredAnimation = FallAnimation;
		break;
	case EEnemyCharacterState::C_DEAD:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		if (MarkedToDestroy)
			DesiredAnimation = FlickeringDeadAnimation;
		else
			DesiredAnimation = DeadAnimation;
		break;
	case EEnemyCharacterState::C_BLOCK:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		break;
	case EEnemyCharacterState::C_GROUND:
		Weapon->SetFlipbook(BlankFlipbook);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		break;
	}

	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCharacter();
	
	EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Float>(EnemyAI->WaitTime, FMath::RandRange(1.0f, 3.0f));

}

void AEnemy::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	Cast<UProgressBar>(HealthWidgetComp->GetUserWidgetObject()->GetWidgetFromName("ProgressBar_142"))->SetPercent(currentHealth);

	if (currentHealth <= 0.0f && EnemyState != EEnemyCharacterState::C_DEAD && EnemyState != EEnemyCharacterState::C_FALL) // We are dead, so we fall and then die
	{
		EnemyState = EEnemyCharacterState::C_FALL;
		float TimelineLength = FallAnimation->GetNumFrames() / (float)FallAnimation->GetFramesPerSecond();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::HandleDead, TimelineLength - 0.05f*TimelineLength, false);
	}

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;

	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	const UEnum* ptr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyCharacterState"), true);
	auto index = ptr->GetIndexByValue((uint8)EnemyState);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, ptr->GetEnumText(index).ToString());

	// Are we moving or standing still?
	if (EnemyState == EEnemyCharacterState::C_RUN || EnemyState == EEnemyCharacterState::C_IDLE) {
		if (PlayerSpeedSqr > 0.0f)
		{
			EnemyState = EEnemyCharacterState::C_RUN;
		}
		else {
			EnemyState = EEnemyCharacterState::C_IDLE;
		}
	}
	
	if (this->GetActorRotation().Yaw != 0.0f)
	{
		FacingRight = false;
		HealthWidgetComp->SetRelativeLocation(FVector(-100.0f, 0.0f, -230.0f));
	}
	else
	{
		FacingRight = true;
		HealthWidgetComp->SetRelativeLocation(FVector(100.0f, 0.0f, -230.0f));
	}

	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyAI = Cast<ABasicEnemyAI>(GetController());
	EnemyAI->GetBlackboardComp()->SetValue<UBlackboardKeyType_Float>(EnemyAI->WaitTime, 1.0f);

}