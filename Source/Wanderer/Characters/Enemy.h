// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperCharacter.h"
#include "Enemy.generated.h"

/**
* Character state
*/
UENUM(BlueprintType)
enum class EEnemyCharacterState : uint8
{
	C_IDLE	UMETA(DisplayName = "Idle"),
	C_RUN	UMETA(DisplayName = "Run"),
	C_JUMP	UMETA(DisplayName = "Jump"),
	C_FIGHT UMETA(DisplayName = "Fight"),
	C_FALL	UMETA(DisplayName = "Fall"),
	C_GROUND	UMETA(DisplayName = "Ground"),
	C_DEAD	UMETA(DisplayName = "Dead"),
	C_BLOCK	UMETA(DisplayName = "Block"),
	C_HIT	UMETA(DisplayName = "Hit"),
	C_HITHOLD	UMETA(DisplayName = "HitHold"),
	C_TALK UMETA(DisplayName = "Talk")
};

/**
 * 
 */
UCLASS()
class WANDERER_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Talk Widget")
	class UWidgetComponent* HealthWidgetComp;


	class UClass* HealthWidgetClass;

	UFUNCTION()
	virtual	void Attack();

	virtual void BeginPlay() override;

	enum class EEnemyCharacterState EnemyState;

	AEnemy();

	UPROPERTY(EditAnywhere, Category = "Behavior Tree")
	class UBehaviorTree* behaviorTree;

	void ReceiveDamage(float DamageIn, bool AirThrow, bool Fall, bool ThrowDirection);

	UFUNCTION()
	void InflictDamage(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		float currentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		float Health;

	UFUNCTION()
		void HandleAttack();

protected:

	virtual void Tick(float DeltaSeconds) override;

	bool MarkedToDestroy;

	class ABasicEnemyAI* EnemyAI;

	bool FacingRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	UFUNCTION()
	void HandleDead();

	UFUNCTION()
	void HandleHit();

	UFUNCTION()
	void HandleHitWithAir();



	UFUNCTION()
	void HandleDestroy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	class USoundCue* FootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	class USoundCue* AttackedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	class USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	class USoundCue* IdleSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	class USoundCue* FallSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Damage;

	// The animation to play while running 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* AttackAnimation;

	// Blank flipbook, servers different purposes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* BlankFlipbook;

	// Flipbook used to calculate collision while attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* AttackAnimationCollision;

	// The animation to play while falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* FallAnimation;

	// The animation to play while being hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* HitAnimation;

	// The animation to play while holding a hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* HitHoldAnimation;

	// The animation to play when you die
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DeadAnimation;

	// The animation to play once dead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* FlickeringDeadAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	// Called when the capsule touchs ground
	void Landed(const FHitResult& Hit) override;

	/** Called to choose the correct animation to play based on the character's state */
	void UpdateAnimation();
	
	void UpdateSound();

	/** Updates character's basic states */
	void UpdateCharacter();

	// Overridable functions to modify behaviour in sub-classes
	virtual void LoadFlipbooks();

	virtual void ConfigureMovement();

	virtual void LoadAI();

	virtual void LoadSounds();
};
