// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PaperCharacter.h"
#include "WandererCharacter.generated.h"

class UTextRenderComponent;

/**
* Character state
*/
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	C_INTROHOLD	UMETA(DisplayName = "IntroHold"),
	C_INTRO	UMETA(DisplayName = "Intro"),
	C_IDLE	UMETA(DisplayName = "Idle"),
	C_RUN	UMETA(DisplayName = "Run"),
	C_JUMP	UMETA(DisplayName = "Jump"),
	C_JUMPFALL	UMETA(DisplayName = "JumpFall"),
	C_JUMPFALLSTATIC	UMETA(DisplayName = "JumpFallStatic"),
	C_JUMPRISE	UMETA(DisplayName = "JumpRise"),
	C_FALL	UMETA(DisplayName = "Fall"),
	C_HIT	UMETA(DisplayName = "Hit"),
	C_GROUND	UMETA(DisplayName = "Ground"),
	C_DEAD	UMETA(DisplayName = "Dead"),
	C_DODGEFORWARD	UMETA(DisplayName = "DodgeForward"),
	C_DODGEBACKWARDS	UMETA(DisplayName = "DodgeBackWards"),
	C_BLOCK	UMETA(DisplayName = "Block"),
	C_BLOCKHOLD	UMETA(DisplayName = "BlockHold"),
	C_COMBO1 UMETA(DisplayName = "Combo1"),
	C_COMBO2 UMETA(DisplayName = "Combo2"),
	C_COMBO3 UMETA(DisplayName = "Combo3"),
	C_UPPERHIT UMETA(DisplayName = "UpperHit"),
	C_TALK UMETA(DisplayName = "Talk")
};

UCLASS(config=Game)
class AWandererCharacter : public APaperCharacter
{
	GENERATED_BODY()

		// Different sprite composition depths weapon collision and sprites must be on the same depth, for overlapping purposes
	bool IntroMode;
	float SpriteDepth;
	float EnheduannaDepth;
	float EffectDepth;
	float GlowDepth;
	float AlternateSpriteDepth;
	float AlternateEnheduannaDepth;
	float AlternateEffectDepth;
	float AlternateGlowDepth;

	/*
	Handle Functions, they execute after certain animation finishes
	*/
	UFUNCTION()
	void HandleAnimation();

	UFUNCTION()
	void HandleJumpFall();

	UFUNCTION()
	void HandleJumpFallStatic();

	UFUNCTION()
	void HandleJumpRise();

	UFUNCTION()
	void HandleDodge();

	UFUNCTION()
	void HandleCombat();

	UFUNCTION()
	void HandleEffect();

	UFUNCTION()
	void HandleBlock();

	UFUNCTION()
	void HandleHit();

	UFUNCTION()
	void HandleUpperHit();
	
	/** Timeline needed to create Enheduanna's pulse */
	UPROPERTY()
	class UTimelineComponent* PulseTimeline;

	UPROPERTY()
	class UCurveFloat* PulseCurve;

	FOnTimelineFloat PulseInterp {};

	UFUNCTION()
	void Pulse(float Value);

	UMaterialInstanceDynamic* DynamicPulse;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* WeaponTrailParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ShieldParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* HitParticle;

	TArray<UParticleSystemComponent*> ParticleSystemArray;

	/** Spline component for the weapon trail */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* WeaponTrailComponent;

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* RiseVoiceAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* EnheduannaAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* BlockingShieldSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* Footstep1Sound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* Footstep2Sound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* Attack1Sound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* Attack2Sound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* Attack3Sound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* AttackedSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* AttackEnheduannaSound;

	/** Flipbook that represents the Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* Enheduanna;

	/** Flipbook that represents the Weapon Collision */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* Weapon;

	/** Flipbook that represents effects */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* EnheduannaGlow;

	/** Flipbook that represents effects */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* Effect;

	UTextRenderComponent* TextComponent;
	virtual void Tick(float DeltaSeconds) override;

protected:

	/** Player statistics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float currentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float Health;

	/** Internal Check Variables */
	int ComboStatus;
	bool BlockAnimationFinished;
	bool BlockButtonReleased;
	bool KeepCombo;
	bool Run;
	bool jumping;
	bool FacingRight;
	bool CanAirCombat;

	/** Input Streaming Variables */
	float XAxisValue;
	float YAxisValue;

	/** Priority Levels */
	// Input Management Level -> 0 means no input, 1 means restricted input, 2 means full input
	int InputManagementLevel;
	// Handle Management Level -> 0 means override all Handle Functions, except Priority 0 ones (Dead)
	// Handle Management Level -> 1 means override Level > 1 Handle Functions (Priority 1 ones are Hit, HitHold and Fall)
	// Handle Management Level -> 2 means override Level > 2 Handle Functions (Priority 2 ones are Combat states)
	// Handle Management Level -> 3 means override Level > 3 Handle Functions (Priority 3 ones are Jump states)
	int HandleManagementLevel;

	/** Pointers to the different character animations */

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IntroAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IntroHoldAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DodgeForwardAnimation;

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* EffectAnimation;

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* WalkingAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* BlankFlipbook;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* Weapon1Animation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* Weapon2Animation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* Weapon3Animation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* UpperHitAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* UpperHitAnimationCollision;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* Weapon1AnimationCollision;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* Weapon2AnimationCollision;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* Weapon3AnimationCollision;

	// The animation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* HitAnimation;

	// The animation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAnimation;

	// The animation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpFallAnimation;

	// The animation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpFallStaticAnimation;

	// The animation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpRiseAnimation;

	// The animation to play while falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* FallAnimation;

	// The animation to play while blocking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* BlockAnimation;

	// The animation to play while blocking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* BlockHoldAnimation;

	// The GlowAnimation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* RunningGlowAnimation;

	// The GlowAnimation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* DodgeForwardGlowAnimation;

	// The GlowAnimation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* WalkingGlowAnimation;

	// The GlowAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* IdleGlowAnimation;

	// The GlowAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* Weapon1GlowAnimation;

	// The GlowAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* Weapon2GlowAnimation;

	// The GlowAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* Weapon3GlowAnimation;

	// The GlowAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* UpperHitGlowAnimation;

	// The GlowAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* HitGlowAnimation;

	// The GlowAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* JumpGlowAnimation;

	// The GlowAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* JumpFallGlowAnimation;

	// The GlowAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* JumpFallStaticGlowAnimation;

	// The GlowAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* JumpRiseGlowAnimation;

	// The GlowAnimation to play while falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* FallGlowAnimation;

	// The GlowAnimation to play while blocking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* BlockGlowAnimation;

	// The GlowAnimation to play while blocking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GlowAnimations)
	class UPaperFlipbook* BlockHoldGlowAnimation;

	// The EnheduannaAnimation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* RunningEnheduannaAnimation;

	// The EnheduannaAnimation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* DodgeForwardEnheduannaAnimation;

	// The EnheduannaAnimation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* WalkingEnheduannaAnimation;

	// The EnheduannaAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* IdleEnheduannaAnimation;

	// The EnheduannaAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* Weapon1EnheduannaAnimation;

	// The EnheduannaAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* Weapon2EnheduannaAnimation;

	// The EnheduannaAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* Weapon3EnheduannaAnimation;

	// The EnheduannaAnimation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* UpperHitEnheduannaAnimation;

	// The EnheduannaAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* HitEnheduannaAnimation;

	// The EnheduannaAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* JumpEnheduannaAnimation;

	// The EnheduannaAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* JumpFallEnheduannaAnimation;

	// The EnheduannaAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* JumpFallStaticEnheduannaAnimation;

	// The EnheduannaAnimation to play while jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* JumpRiseEnheduannaAnimation;

	// The EnheduannaAnimation to play while falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* FallEnheduannaAnimation;

	// The EnheduannaAnimation to play while blocking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* BlockEnheduannaAnimation;

	// The EnheduannaAnimation to play while blocking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnheduannaAnimations)
	class UPaperFlipbook* BlockHoldEnheduannaAnimation;

	/** Called to choose the correct animation to play based on the character's state */
	void UpdateAnimation();

	void UpdateSound();

	/** Called for side to side input */
	void MoveRight(float Value);

	void VerticalStick(float Value);

	/** Called for settings menu */
	void SettingsMenu();

	void Landed(const FHitResult& Hit) override;

	void Jump();

	void Interact();

	void Dodge();

	void BlockRelease();
	void Block();

	void UpdateCharacter();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:

	bool IsFacingRight();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
		float startopacity;

	/** Character movement state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	ECharacterState CharacterState;

	void ReceiveDamage(float DamageIn, bool AirThrow, bool Fall);

	UFUNCTION()
	void InflictDamage(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Dialogue character status -> should be integrated into character states? */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Dialogue, meta = (AllowPrivateAccess = "true"))
	bool IsTalking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Dialogue, meta = (AllowPrivateAccess = "true"))
	bool CanTalk;

	AWandererCharacter();

	virtual void BeginPlay() override;

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
