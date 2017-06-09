// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Wanderer.h"
#include "Characters/WandererCharacter.h"
#include "GameMode/WandererGameMode.h"
#include "PaperFlipbookComponent.h"
#include "GameMode/WandererHUD.h"
#include "Characters/Enemy.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Components/TextRenderComponent.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);
//////////////////////////////////////////////////////////////////////////
// AWandererCharacter

AWandererCharacter::AWandererCharacter()
{
	startopacity = 0.0f;
	//IntroMode = true;
	// Components Depth
	SpriteDepth = 0.0f; // Needed for collision
	EnheduannaDepth = 2.0; // Needed for collision
	EffectDepth = 3.0f;
	GlowDepth = 1.0f;
	AlternateEnheduannaDepth = -1.0f;
	AlternateEffectDepth = 1.0f;
	AlternateGlowDepth = -2.0f;


	// Priority Levels Initialization
	HandleManagementLevel = 3;
	InputManagementLevel = 2;

	// Stats Variables Initialization
	Health = 1.0f;
	currentHealth = Health;

	// Input Streaming Values Initialization
	XAxisValue = 0;
	YAxisValue = 0;

	// Check Variables Initialization
	ComboStatus = 0;
	jumping = false;
	CanTalk = false;
	IsTalking = false;
	KeepCombo = false;
	Run = false;
	FacingRight = true;
	BlockAnimationFinished = true;
	BlockButtonReleased = false;
	CanAirCombat = true;

	// Timeline initialization
	static ConstructorHelpers::FObjectFinder<UCurveFloat> PulseCurveReference(TEXT("CurveFloat'/Game/2DSideScroller/Materials/PulseCurve.PulseCurve'"));
	if (PulseCurveReference.Object) {
		PulseCurve = PulseCurveReference.Object;
	}

	PulseTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PulseTimeline"));

	PulseInterp.BindUFunction(this, FName{ TEXT("Pulse") });

	// State Initialization
	if (IntroMode == false)
	CharacterState = ECharacterState::C_IDLE;
	else
		CharacterState = ECharacterState::C_INTRO;

	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Creating the spline component
	WeaponTrailComponent = CreateDefaultSubobject<USplineComponent>(TEXT("WeaponTrailComponent"));
	WeaponTrailComponent->SetupAttachment(RootComponent);

	// Fill spline component
	WeaponTrailComponent->AddSplineLocalPoint(FVector(49.0f, 60.0f, -78.0f));
	WeaponTrailComponent->AddSplineLocalPoint(FVector(77.51f, 60.0f, -57.73f));
	WeaponTrailComponent->AddSplineLocalPoint(FVector(112.0f, 60.0f, -44.5f));
	WeaponTrailComponent->AddSplineLocalPoint(FVector(170.0f, 60.0f, 10.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemReference(TEXT("ParticleSystem'/Game/2DSideScroller/Particles/P_Beam.P_Beam'"));
	WeaponTrailParticle = ParticleSystemReference.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> HitParticleReference(TEXT("ParticleSystem'/Game/2DSideScroller/Particles/BurstParticle.BurstParticle'"));
	HitParticle = HitParticleReference.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ShieldParticleReference(TEXT("ParticleSystem'/Game/2DSideScroller/Particles/ShieldEffect.ShieldEffect'"));
	ShieldParticle = ShieldParticleReference.Object;

	// Create the audio component
	RiseVoiceAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RiseVoiceAudioComponent"));
	RiseVoiceAudioComponent->bAutoActivate = false;
	RiseVoiceAudioComponent->SetupAttachment(RootComponent);

	EnheduannaAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EnheduannaAudioComponent"));
	EnheduannaAudioComponent->bAutoActivate = false;
	EnheduannaAudioComponent->SetupAttachment(RootComponent);

	// Loading sound assets
	
	static ConstructorHelpers::FObjectFinder<USoundCue> BlockingShieldSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Rise-ShieldSound.Rise-ShieldSound'"));
	BlockingShieldSound = BlockingShieldSoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> Footstep1Reference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Running/Footstep1-Cue.Footstep1-Cue'"));
	Footstep1Sound = Footstep1Reference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> Footstep2Reference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Running/Footstep2-Cue.Footstep2-Cue'"));
	Footstep2Sound = Footstep2Reference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> Attack1SoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Rise-Attack1Sound.Rise-Attack1Sound'"));
	Attack1Sound = Attack1SoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> Attack2SoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Rise-Attack2Sound.Rise-Attack2Sound'"));
	Attack2Sound = Attack2SoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> Attack3SoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Rise-Attack3Sound.Rise-Attack3Sound'"));
	Attack3Sound = Attack3SoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> AttackedSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Rise-AttackedSound.Rise-AttackedSound'"));
	AttackedSound = AttackedSoundReference.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> AttackEnheduannaSoundReference(TEXT("SoundCue'/Game/2DSideScroller/Sounds/Rise/Rise-EnheduannaAttackSound.Rise-EnheduannaAttackSound'"));
	AttackEnheduannaSound = AttackEnheduannaSoundReference.Object;

	// Create glow flipbook and attach it
	EnheduannaGlow = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Glow"));
	EnheduannaGlow->SetupAttachment(RootComponent);
	EnheduannaGlow->SetIsReplicated(true);
	EnheduannaGlow->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

	// Create the weapon flipbook and attach it to the root
	Weapon = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("WeaponCollision"));
	Weapon->SetupAttachment(RootComponent);
	Weapon->SetIsReplicated(true);
	Weapon->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	Weapon->SetHiddenInGame(true);

	Enheduanna = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Weapon"));
	Enheduanna->SetupAttachment(RootComponent);
	Enheduanna->SetIsReplicated(true);
	Enheduanna->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

	Effect = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Effect"));
	Effect->SetupAttachment(RootComponent);
	Effect->SetIsReplicated(true);
	Effect->SetWorldScale3D(FVector(0.15f, 0.15f, 0.15f));

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 482.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->TargetOffset = FVector(200.0f, 0.0f, 0.0f);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	//CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 1.0f;

	// Create a perspective camera and attach it to the camera boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Perspective;
	SideViewCameraComponent->FieldOfView = 90;
	SideViewCameraComponent->bConstrainAspectRatio = 1;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->bAbsoluteRotation = true;
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	GetSprite()->bGenerateOverlapEvents = 1;
	GetSprite()->CastShadow = 1;
	GetSprite()->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	bReplicates = true;

	this->AutoReceiveInput = EAutoReceiveInput::Player0;
	// Dinamically link a begin overlap event
	Weapon->OnComponentBeginOverlap.AddDynamic(this, &AWandererCharacter::InflictDamage);

	// Loading specific animations

	// For the animation component itself
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IntroAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Textures/Rise/Intro/Intro.Intro'"));
	IntroAnimation = IntroAnimationReference.Get();

	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IntroHoldAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Textures/Rise/Intro/IntroHold.IntroHold'"));
	IntroHoldAnimation = IntroHoldAnimationReference.Get();

	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Run.Rise-Run'"));
	RunningAnimation = RunningAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DodgeForwardAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Dodge.Rise-Dodge'"));
	DodgeForwardAnimation = DodgeForwardAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> EffectAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Effects/Materialize.Materialize'"));
	EffectAnimation = EffectAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> WalkingAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Walk.Rise-Walk'"));
	WalkingAnimation = WalkingAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Idle.Rise-Idle'"));
	IdleAnimation = IdleAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlankFlipbookReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/BlankFlipbook.BlankFlipbook'"));
	BlankFlipbook = BlankFlipbookReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon1AnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon1.Rise-Weapon1'"));
	Weapon1Animation = Weapon1AnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon2AnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon2.Rise-Weapon2'"));
	Weapon2Animation = Weapon2AnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon3AnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon3.Rise-Weapon3'"));
	Weapon3Animation = Weapon3AnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> UpperHitAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-UpperHit.Rise-UpperHit'"));
	UpperHitAnimation = UpperHitAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> UpperHitAnimationCollisionReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-UpperHitCollision.Rise-UpperHitCollision'"));
	UpperHitAnimationCollision = UpperHitAnimationCollisionReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon1AnimationCollisionReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon1Collision.Rise-Weapon1Collision'"));
	Weapon1AnimationCollision = Weapon1AnimationCollisionReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon2AnimationCollisionReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon2Collision.Rise-Weapon2Collision'"));
	Weapon2AnimationCollision = Weapon2AnimationCollisionReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon3AnimationCollisionReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon3Collision.Rise-Weapon3Collision'"));
	Weapon3AnimationCollision = Weapon3AnimationCollisionReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Hit.Rise-Hit'"));
	HitAnimation = HitAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Jump.Rise-Jump'"));
	JumpAnimation = JumpAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpFallAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpFall.Rise-JumpFall'"));
	JumpFallAnimation = JumpFallAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpFallStaticAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpFallStatic.Rise-JumpFallStatic'"));
	JumpFallStaticAnimation = JumpFallStaticAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpRiseAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpRise.Rise-JumpRise'"));
	JumpRiseAnimation = JumpRiseAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlockAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Block.Rise-Block'"));
	BlockAnimation = BlockAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlockHoldAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-BlockHold.Rise-BlockHold'"));
	BlockHoldAnimation = BlockHoldAnimationReference.Get();

	// Glow flipbooks
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-RunGlow.Rise-RunGlow'"));
	RunningGlowAnimation = RunningGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DodgeForwardGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-DodgeGlow.Rise-DodgeGlow'"));
	DodgeForwardGlowAnimation = DodgeForwardGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> WalkingGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-WalkGlow.Rise-WalkGlow'"));
	WalkingGlowAnimation = WalkingGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-IdleGlow.Rise-IdleGlow'"));
	IdleGlowAnimation = IdleGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon1GlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon1Glow.Rise-Weapon1Glow'"));
	Weapon1GlowAnimation = Weapon1GlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon2GlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon2Glow.Rise-Weapon2Glow'"));
	Weapon2GlowAnimation = Weapon2GlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon3GlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon3Glow.Rise-Weapon3Glow'"));
	Weapon3GlowAnimation = Weapon3GlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> UpperHitGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-UpperHitGlow.Rise-UpperHitGlow'"));
	UpperHitGlowAnimation = UpperHitGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-HitGlow.Rise-HitGlow'"));
	HitGlowAnimation = HitGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpGlow.Rise-JumpGlow'"));
	JumpGlowAnimation = JumpGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpFallGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpFallGlow.Rise-JumpFallGlow'"));
	JumpFallGlowAnimation = JumpFallGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpFallStaticGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpFallStaticGlow.Rise-JumpFallStaticGlow'"));
	JumpFallStaticGlowAnimation = JumpFallStaticGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpRiseGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpRiseGlow.Rise-JumpRiseGlow'"));
	JumpRiseGlowAnimation = JumpRiseGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlockGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-BlockGlow.Rise-BlockGlow'"));
	BlockGlowAnimation = BlockGlowAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlockHoldGlowAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-BlockHoldGlow.Rise-BlockHoldGlow'"));
	BlockHoldGlowAnimation = BlockHoldGlowAnimationReference.Get();

	// Enheduanna flipbooks
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-RunEnheduanna.Rise-RunEnheduanna'"));
	RunningEnheduannaAnimation = RunningEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DodgeForwardEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-DodgeEnheduanna.Rise-DodgeEnheduanna'"));
	DodgeForwardEnheduannaAnimation = DodgeForwardEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> WalkingEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-WalkEnheduanna.Rise-WalkEnheduanna'"));
	WalkingEnheduannaAnimation = WalkingEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-IdleEnheduanna.Rise-IdleEnheduanna'"));
	IdleEnheduannaAnimation = IdleEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon1EnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon1Enheduanna.Rise-Weapon1Enheduanna'"));
	Weapon1EnheduannaAnimation = Weapon1EnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon2EnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon2Enheduanna.Rise-Weapon2Enheduanna'"));
	Weapon2EnheduannaAnimation = Weapon2EnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Weapon3EnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-Weapon3Enheduanna.Rise-Weapon3Enheduanna'"));
	Weapon3EnheduannaAnimation = Weapon3EnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> UpperHitEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-UpperHitEnheduanna.Rise-UpperHitEnheduanna'"));
	UpperHitEnheduannaAnimation = UpperHitEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> HitEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-HitEnheduanna.Rise-HitEnheduanna'"));
	HitEnheduannaAnimation = HitEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpEnheduanna.Rise-JumpEnheduanna'"));
	JumpEnheduannaAnimation = JumpEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpFallEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpFallEnheduanna.Rise-JumpFallEnheduanna'"));
	JumpFallEnheduannaAnimation = JumpFallEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpFallStaticEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpFallStaticEnheduanna.Rise-JumpFallStaticEnheduanna'"));
	JumpFallStaticEnheduannaAnimation = JumpFallStaticEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpRiseEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-JumpRiseEnheduanna.Rise-JumpRiseEnheduanna'"));
	JumpRiseEnheduannaAnimation = JumpRiseEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlockEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-BlockEnheduanna.Rise-BlockEnheduanna'"));
	BlockEnheduannaAnimation = BlockEnheduannaAnimationReference.Get();
	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlockHoldEnheduannaAnimationReference(TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Rise/Rise-BlockHoldEnheduanna.Rise-BlockHoldEnheduanna'"));
	BlockHoldEnheduannaAnimation = BlockHoldEnheduannaAnimationReference.Get();

	// Collision Presets
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetSprite()->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Weapon->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Effect->SetCollisionProfileName(TEXT("NoCollision"));

	// Set the main flipbook material
	UMaterial* material = LoadObject<UMaterial>(NULL, TEXT("Material'/Paper2D/DefaultLitSpriteMaterial.DefaultLitSpriteMaterial'"));
	GetSprite()->SetMaterial(0, material);
	Enheduanna->SetMaterial(0, material);

	UMaterialInstance* material2 = LoadObject<UMaterialInstanceConstant>(NULL, TEXT("MaterialInstanceConstant'/Game/2DSideScroller/Materials/EnheduannaGlow.EnheduannaGlow'"));

	DynamicPulse = UMaterialInstanceDynamic::Create(material2, NULL);
	Effect->SetMaterial(0, DynamicPulse);
	EnheduannaGlow->SetMaterial(0, DynamicPulse);
}

void AWandererCharacter::Pulse(float Value)
{
	DynamicPulse->SetScalarParameterValue("Glow", Value);
	Effect->SetMaterial(0, DynamicPulse);
	EnheduannaGlow->SetMaterial(0, DynamicPulse);
}

void AWandererCharacter::ReceiveDamage(float DamageIn, bool AirThrow, bool Fall)
{
	if(CharacterState != ECharacterState::C_HIT && CharacterState != ECharacterState::C_DODGEFORWARD) {

		if (DamageIn == 0.0f) // We are blocking
		{
			EnheduannaAudioComponent->SetSound(BlockingShieldSound);
			EnheduannaAudioComponent->Play();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldParticle, FTransform(this->GetActorLocation()), true);
		}
		else {
			jumping = false;

			float TimelineLength = HitAnimation->GetNumFrames() / (float)HitAnimation->GetFramesPerSecond();
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleHit, TimelineLength - 0.05f*TimelineLength, false);
				
			RiseVoiceAudioComponent->SetSound(AttackedSound);
			RiseVoiceAudioComponent->Play();

			CharacterState = ECharacterState::C_HIT;
			currentHealth = currentHealth - DamageIn;
			InputManagementLevel = 1;
			HandleManagementLevel = 1;
		}
	}
}

void AWandererCharacter::InflictDamage(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CharacterState != ECharacterState::C_BLOCK && CharacterState != ECharacterState::C_BLOCKHOLD) // We are fighting
	{
	// We check if the current frame is the last one in the animation
	//if (Weapon->GetPlaybackPositionInFrames() == Weapon->GetFlipbook()->GetNumFrames() - 1) {
		if (Cast<AEnemy>(OtherActor) && OtherComp->GetName() == "CollisionCylinder")
		{
			/*
			TArray<FHitResult> AllResults;

			// Get the location of this actor
			auto Start = GetActorLocation();
			// Get the location of the other component
			auto End = OtherComp->GetComponentLocation();
			// Use a slightly larger radius to ensure we find the same result
			auto CollisionRadius = FVector::Dist(Start, End) * 1.1f;

			// Now do a spherical sweep to find the overlap
			GetWorld()->SweepMultiByObjectType(
				AllResults,
				Start,
				End,
				FQuat::Identity,
				0,
				FCollisionShape::MakeSphere(CollisionRadius),
				FCollisionQueryParams::FCollisionQueryParams(false)
				);

			// Finally check which hit result is the one from this event
			for (auto HitResult : AllResults)
			{
				if (OtherComp->GetUniqueID() == HitResult.GetComponent()->GetUniqueID()) {
					// A component with the same UniqueID means we found our overlap!

				

					break;
				}
			}
			*/

			if (CharacterState == ECharacterState::C_UPPERHIT)
			{
				if (Cast<AEnemy>(OtherActor)->EnemyState != EEnemyCharacterState::C_HIT && Cast<AEnemy>(OtherActor)->EnemyState != EEnemyCharacterState::C_HITHOLD) // Inflict the damange only if the enemy is not in a hit state
				{
					Cast<AEnemy>(OtherActor)->ReceiveDamage(0.15f, true, false, true);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, FTransform(OtherComp->GetComponentLocation()), true);
				}
			}
			else
			{
				if (this->GetActorLocation().X >= OtherActor->GetActorLocation().X)
				{
					// Se mueve hacia la izquierda
					if (Cast<AEnemy>(OtherActor)->EnemyState != EEnemyCharacterState::C_HIT && Cast<AEnemy>(OtherActor)->EnemyState != EEnemyCharacterState::C_HITHOLD) // Inflict the damange only if the enemy is not in a hit state
					{
						Cast<AEnemy>(OtherActor)->ReceiveDamage(0.15f, false, false, true);
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, FTransform(OtherComp->GetComponentLocation()), true);
					}
				}
				else
				{
					// Se mueve a la drecha
					if (Cast<AEnemy>(OtherActor)->EnemyState != EEnemyCharacterState::C_HIT && Cast<AEnemy>(OtherActor)->EnemyState != EEnemyCharacterState::C_HITHOLD) // Inflict the damange only if the enemy is not in a hit state
					{
						Cast<AEnemy>(OtherActor)->ReceiveDamage(0.15f, false, false, false);
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, FTransform(OtherComp->GetComponentLocation()), true);
					}
				}
			}

		}
		else
		{
		}	
	//}
	}
}

void AWandererCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetController()->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
	PulseTimeline->AddInterpFloat(PulseCurve, PulseInterp, FName{ TEXT("Pulse") });
	PulseTimeline->SetLooping(true);
	PulseTimeline->Play();



}

void AWandererCharacter::HandleHit()
{
	if (HandleManagementLevel >= 1)
	{
		CharacterState = ECharacterState::C_IDLE;
		InputManagementLevel = 2;
		HandleManagementLevel = 3;
	}
}
//////////////////////////////////////////////////////////////////////////
// Animation

/** Animation update logic, should NOT modify character's state
*   Logic should be based on character's state -> use a switch?
*/
void AWandererCharacter::UpdateAnimation()
{

	UPaperFlipbook* DesiredAnimation = nullptr;

	switch (CharacterState) {
	case ECharacterState::C_INTRO:
		// Flipbook component local location
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		Enheduanna->SetFlipbook(BlankFlipbook);
		EnheduannaGlow->SetFlipbook(BlankFlipbook);
		DesiredAnimation = IntroAnimation;

		break;
	case ECharacterState::C_INTROHOLD:
		// Flipbook component local location
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		Enheduanna->SetFlipbook(BlankFlipbook);
		EnheduannaGlow->SetFlipbook(BlankFlipbook);
		DesiredAnimation = IntroHoldAnimation;

		break;

	case ECharacterState::C_IDLE:
		// Flipbook component local location
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		Enheduanna->SetFlipbook(IdleEnheduannaAnimation);
		EnheduannaGlow->SetFlipbook(IdleGlowAnimation);
		DesiredAnimation = IdleAnimation;

		break;
	case ECharacterState::C_RUN:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);

		if (Run)
		{
			if ((FacingRight && XAxisValue < 0.75) || (FacingRight == false && XAxisValue > -0.75)) // Depending on how we move de axis, play full rate animation or not
			{
				GetCharacterMovement()->MaxWalkSpeed = 500.0f;
				GetSprite()->SetPlayRate(0.5f);
				Enheduanna->SetPlayRate(0.5f);
				EnheduannaGlow->SetPlayRate(0.5f);
			}
			else {
				GetCharacterMovement()->MaxWalkSpeed = 700.0f;
				GetSprite()->SetPlayRate(1.0f);
				Enheduanna->SetPlayRate(1.0f);
				EnheduannaGlow->SetPlayRate(1.0f);
			}

			DesiredAnimation = RunningAnimation;
			Enheduanna->SetFlipbook(RunningEnheduannaAnimation);
			EnheduannaGlow->SetFlipbook(RunningGlowAnimation);
		}
		else 
		{
			DesiredAnimation = WalkingAnimation;
			Enheduanna->SetFlipbook(WalkingEnheduannaAnimation);
			EnheduannaGlow->SetFlipbook(WalkingGlowAnimation);
		}
			
		break;
	case ECharacterState::C_JUMP:
		// Flipbook component local location
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, AlternateGlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, AlternateEnheduannaDepth, -100.0f));
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = JumpAnimation;
		EnheduannaGlow->SetFlipbook(JumpGlowAnimation);
		Enheduanna->SetFlipbook(JumpEnheduannaAnimation);

		break;
	case ECharacterState::C_JUMPFALL:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, AlternateGlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, AlternateEnheduannaDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = JumpFallAnimation;
		EnheduannaGlow->SetFlipbook(JumpFallGlowAnimation);
		Enheduanna->SetFlipbook(JumpFallEnheduannaAnimation);

		break;
	case ECharacterState::C_JUMPFALLSTATIC:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, AlternateGlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, AlternateEnheduannaDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = JumpFallStaticAnimation;
		EnheduannaGlow->SetFlipbook(JumpFallStaticGlowAnimation);
		Enheduanna->SetFlipbook(JumpFallStaticEnheduannaAnimation);

		break;
	case ECharacterState::C_JUMPRISE:
		// Flipbook component local location
		Enheduanna->SetRelativeLocation(FVector(0.0f, AlternateEnheduannaDepth, -100.0f));
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, AlternateGlowDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = JumpRiseAnimation;
		EnheduannaGlow->SetFlipbook(JumpRiseGlowAnimation);
		Enheduanna->SetFlipbook(JumpRiseEnheduannaAnimation);

		break;
	case ECharacterState::C_COMBO1:
		// Flipbook component local location
		Enheduanna->SetRelativeLocation(FVector(70.0f, EnheduannaDepth, -130.0f));
		Weapon->SetRelativeLocation(FVector(70.0f, SpriteDepth, -130.0f));
		GetSprite()->SetRelativeLocation(FVector(70.0f, SpriteDepth, -130.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(70.0f, GlowDepth, -130.0f));

		// Flipbook selection
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = Weapon1Animation;
		Weapon->SetFlipbook(Weapon1AnimationCollision);
		EnheduannaGlow->SetFlipbook(Weapon1GlowAnimation);
		Enheduanna->SetFlipbook(Weapon1EnheduannaAnimation);

		break;
	case ECharacterState::C_UPPERHIT:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		Weapon->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));

		// Flipbook selection
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = UpperHitAnimation;
		Weapon->SetFlipbook(UpperHitAnimationCollision);
		EnheduannaGlow->SetFlipbook(UpperHitGlowAnimation);
		Enheduanna->SetFlipbook(UpperHitEnheduannaAnimation);

		break;
	case ECharacterState::C_COMBO2:
		// Flipbook component local location
		Enheduanna->SetRelativeLocation(FVector(70.0f, EnheduannaDepth, -130.0f));
		Weapon->SetRelativeLocation(FVector(70.0f, SpriteDepth, -130.0f));
		GetSprite()->SetRelativeLocation(FVector(70.0f, SpriteDepth, -130.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(70.0f, GlowDepth, -130.0f));

		// Flipbook selection
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = Weapon2Animation;
		Weapon->SetFlipbook(Weapon2AnimationCollision);
		EnheduannaGlow->SetFlipbook(Weapon2GlowAnimation);
		Enheduanna->SetFlipbook(Weapon2EnheduannaAnimation);

		break;
	case ECharacterState::C_COMBO3:
		// Flipbook component local location
		Enheduanna->SetRelativeLocation(FVector(70.0f, EnheduannaDepth, -130.0f));
		Weapon->SetRelativeLocation(FVector(70.0f, SpriteDepth, -130.0f));
		GetSprite()->SetRelativeLocation(FVector(70.0f, SpriteDepth, -130.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(70.0f, GlowDepth, -130.0f));

		// Flipbook selection
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = Weapon3Animation;
		Weapon->SetFlipbook(Weapon3AnimationCollision);
		EnheduannaGlow->SetFlipbook(Weapon3GlowAnimation);
		Enheduanna->SetFlipbook(Weapon3EnheduannaAnimation);

		break;
	case ECharacterState::C_HIT:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, AlternateGlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, AlternateEnheduannaDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = HitAnimation;
		EnheduannaGlow->SetFlipbook(HitGlowAnimation);
		Enheduanna->SetFlipbook(HitEnheduannaAnimation);

		break;
	case ECharacterState::C_TALK:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = IdleAnimation;
		EnheduannaGlow->SetFlipbook(IdleGlowAnimation);
		Enheduanna->SetFlipbook(IdleEnheduannaAnimation);

		break;
	case ECharacterState::C_FALL:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, AlternateGlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, AlternateEnheduannaDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		break;
	case ECharacterState::C_DEAD:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, AlternateGlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, AlternateEnheduannaDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		break;
	case ECharacterState::C_BLOCK:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));

		// Flipbook selection
		DesiredAnimation = BlockAnimation;
		EnheduannaGlow->SetFlipbook(BlockGlowAnimation);
		Enheduanna->SetFlipbook(BlockEnheduannaAnimation);

		break;
	case ECharacterState::C_BLOCKHOLD:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));

		// Flipbook selection
		DesiredAnimation = BlockHoldAnimation;
		EnheduannaGlow->SetFlipbook(BlockHoldGlowAnimation);
		Enheduanna->SetFlipbook(BlockHoldEnheduannaAnimation);

		break;
	case ECharacterState::C_DODGEFORWARD:
		// Flipbook component local location
		GetSprite()->SetRelativeLocation(FVector(0.0f, SpriteDepth, -100.0f));
		EnheduannaGlow->SetRelativeLocation(FVector(0.0f, GlowDepth, -100.0f));
		Enheduanna->SetRelativeLocation(FVector(0.0f, EnheduannaDepth, -100.0f));

		// Flipbook selection
		Weapon->SetFlipbook(BlankFlipbook);
		Effect->SetFlipbook(BlankFlipbook);
		DesiredAnimation = DodgeForwardAnimation;
		EnheduannaGlow->SetFlipbook(DodgeForwardGlowAnimation);
		Enheduanna->SetFlipbook(DodgeForwardEnheduannaAnimation);

		break;
	}

	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AWandererCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const UEnum* ptr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECharacterState"), true);
	auto index = ptr->GetIndexByValue((uint8)CharacterState);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, ptr->GetEnumText(index).ToString());
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(jumping));

	if (IntroMode == false)
		UpdateCharacter();

	// Update animation to match the motion
	
		UpdateAnimation();
	UpdateSound();
}

void AWandererCharacter::UpdateSound()
{
	switch (CharacterState)
	{
	case ECharacterState::C_RUN:
		if (Run)
		{
			if (GetSprite()->GetPlaybackPositionInFrames() == 0)
			{
				RiseVoiceAudioComponent->SetSound(Footstep1Sound);
				RiseVoiceAudioComponent->Play();

			}
			if (GetSprite()->GetPlaybackPositionInFrames() == 5)
			{
				RiseVoiceAudioComponent->SetSound(Footstep2Sound);
				RiseVoiceAudioComponent->Play();
			}
		}
		else
		{

		}
		break;

	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AWandererCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AWandererCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWandererCharacter::MoveRight);
	PlayerInputComponent->BindAxis("VerticalStick", this, &AWandererCharacter::VerticalStick);
	PlayerInputComponent->BindAction("SettingsMenu", IE_Pressed, this, &AWandererCharacter::SettingsMenu);
	PlayerInputComponent->BindAction("Attack/Interact", IE_Pressed, this, &AWandererCharacter::Interact);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AWandererCharacter::Dodge);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AWandererCharacter::Block);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AWandererCharacter::BlockRelease);
}

void AWandererCharacter::BlockRelease()
{
	BlockButtonReleased = true;

	if (BlockAnimationFinished)
	{
		CharacterState = ECharacterState::C_IDLE;
		InputManagementLevel = 2;
	}
}

void AWandererCharacter::Block()
{

	switch (InputManagementLevel)
	{
	case 2:
	{
		InputManagementLevel = 1; // First, disable full input so we can check wether we are forward or backwards dodging

		BlockButtonReleased = false;
		BlockAnimationFinished = false;
		CharacterState = ECharacterState::C_BLOCK;
		Effect->SetFlipbook(EffectAnimation);
		float TimelineLength2 = EffectAnimation->GetNumFrames() / (float)EffectAnimation->GetFramesPerSecond();
		FTimerHandle TimerHandle2;
		GetWorldTimerManager().SetTimer(TimerHandle2, this, &AWandererCharacter::HandleEffect, TimelineLength2 - 0.05f*TimelineLength2, false);
		FTimerHandle TimerHandle;
		float TimelineLength = BlockAnimation->GetNumFrames() / (float)BlockAnimation->GetFramesPerSecond();
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleBlock, TimelineLength - 0.05f*TimelineLength, false);
	}
	case 1:
		if (FacingRight)
		{
			Effect->SetRelativeLocation(FVector(32.0f, 2.0f, 35.0f));
		}
		else
		{
			Effect->SetRelativeLocation(FVector(32.0f, -2.0f, 35.0f));
		}
	case 0:
		break;
	}
}

void AWandererCharacter::HandleEffect()
{
	Effect->SetFlipbook(BlankFlipbook);
}

void AWandererCharacter::HandleBlock()
{
	if (HandleManagementLevel >= 3)
	{
		if (BlockButtonReleased)
		{
			InputManagementLevel = 2;
			CharacterState = ECharacterState::C_IDLE;
		}
		else
		{
			CharacterState = ECharacterState::C_BLOCKHOLD;
		}

		BlockAnimationFinished = true;
	}
}

void AWandererCharacter::Dodge()
{
	switch (InputManagementLevel)
	{
	case 2:
		if (jumping == false)
		{
			InputManagementLevel = 1; // First, disable full input so we can check wether we are forward or backwards dodging

			if (FacingRight) // Now, if we were facing right
			{
				CharacterState = ECharacterState::C_DODGEFORWARD;
				this->GetCharacterMovement()->AddForce(FVector(29000000.0f, 0.0f, 0.0f));
				float TimelineLength = DodgeForwardAnimation->GetNumFrames() / (float)DodgeForwardAnimation->GetFramesPerSecond();
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleDodge, TimelineLength - 0.05f*TimelineLength, false);
			}
			else // We are facing left
			{
				CharacterState = ECharacterState::C_DODGEFORWARD;
				this->GetCharacterMovement()->AddForce(FVector(-29000000.0f, 0.0f, 0.0f));
				float TimelineLength = DodgeForwardAnimation->GetNumFrames() / (float)DodgeForwardAnimation->GetFramesPerSecond();
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleDodge, TimelineLength - 0.05f*TimelineLength, false);
			}
		}
	case 1:
	case 0:
		break;
	}
}

bool AWandererCharacter::IsFacingRight()
{
	return FacingRight;
}

void AWandererCharacter::Landed(const FHitResult& Hit)
{
	if (HandleManagementLevel >= 3)
	{
		Super::Landed(Hit);
		jumping = false;
		CharacterState = ECharacterState::C_JUMPRISE;
		float TimelineLength = JumpRiseAnimation->GetNumFrames() / (float)JumpRiseAnimation->GetFramesPerSecond();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleJumpRise, TimelineLength - 0.05f*TimelineLength, false);
		this->DisableInput(GetWorld()->GetFirstPlayerController());
		CanAirCombat = true;

	}
	HandleManagementLevel = 3;
}

void AWandererCharacter::HandleJumpFall()
{
	if (HandleManagementLevel >= 3)
	{
		CharacterState = ECharacterState::C_JUMPFALL;
		float TimelineLength = JumpFallAnimation->GetNumFrames() / (float)JumpFallAnimation->GetFramesPerSecond();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleJumpFallStatic, TimelineLength - 0.05f*TimelineLength, false);
	}
}

void AWandererCharacter::HandleJumpFallStatic()
{
	if (HandleManagementLevel >= 3)
		CharacterState = ECharacterState::C_JUMPFALLSTATIC;
}

void AWandererCharacter::HandleDodge()
{
	if (HandleManagementLevel >= 3)
	{
		CharacterState = ECharacterState::C_IDLE;
		if (FacingRight)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
			SpriteDepth = 0.0f; // Needed for collision
			EnheduannaDepth = -2.0; // Needed for collision
			EffectDepth = -3.0f;
			GlowDepth = -1.0f;
			AlternateEnheduannaDepth = 1.0f;
			AlternateEffectDepth = -1.0f;
			AlternateGlowDepth = 2.0f;
			FacingRight = false;
		}
		else
		{
			Controller->SetControlRotation(FRotator(0.0, 0.0f, 0.0f));
			SpriteDepth = 0.0f; // Needed for collision
			EnheduannaDepth = 2.0; // Needed for collision
			EffectDepth = 3.0f;
			GlowDepth = 1.0f;
			AlternateEnheduannaDepth = -1.0f;
			AlternateEffectDepth = 1.0f;
			AlternateGlowDepth = -2.0f;
			FacingRight = true;
		}

		InputManagementLevel = 2;
	}
}

void AWandererCharacter::HandleJumpRise()
{
	if (HandleManagementLevel >= 3)
	{
		CharacterState = ECharacterState::C_IDLE;
		this->EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void AWandererCharacter::Jump()
{
	switch (InputManagementLevel)
	{
	case 2: // full input
	{
		if (jumping == false) 
		{
			CharacterState = ECharacterState::C_JUMP;
			float TimelineLength = JumpAnimation->GetNumFrames() / (float)JumpAnimation->GetFramesPerSecond();
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleJumpFall, TimelineLength - 0.05f*TimelineLength, false);
			Super::Jump();
			jumping = true;
		}
	}
	case 1:
	case 0:
		break;
	}
}

void AWandererCharacter::HandleCombat()
{
	// Weapon Trail

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(WeaponTrailComponent->GetNumberOfSplinePoints()));

	for (int i = 2; i < WeaponTrailComponent->GetNumberOfSplinePoints(); i++)
	{
		UParticleSystemComponent* system = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponTrailParticle, FTransform(WeaponTrailComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World)), true);
		system->SetVectorParameter("Source", WeaponTrailComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
		system->SetVectorParameter("Source_Tan", WeaponTrailComponent->GetTangentAtDistanceAlongSpline(WeaponTrailComponent->GetDistanceAlongSplineAtSplinePoint(i), ESplineCoordinateSpace::World));
		ParticleSystemArray.Add(system);

		system->SetVectorParameter("Target", WeaponTrailComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World));
		system->SetVectorParameter("Target_Tan", WeaponTrailComponent->GetTangentAtDistanceAlongSpline(WeaponTrailComponent->GetDistanceAlongSplineAtSplinePoint(i + 1), ESplineCoordinateSpace::World));
	}

	if (HandleManagementLevel >= 2)
	{
		if (KeepCombo)
		{
			KeepCombo = false;
		}
	}
}

void AWandererCharacter::VerticalStick(float Value)
{
	YAxisValue = Value;
}

void AWandererCharacter::Interact()
{

	switch (InputManagementLevel)
	{
	case 2:
		if (CanTalk) // We are close to some interaction object/NPC, so by context, we'll talk
		{
			CharacterState = ECharacterState::C_TALK;
			IsTalking = true;
		}
		else // We fight
		{
			EnheduannaAudioComponent->SetSound(AttackEnheduannaSound);
			EnheduannaAudioComponent->Play();
			if (YAxisValue == 0 || jumping) // Weapon Combo
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::SanitizeFloat(ComboStatus));

				if (jumping && CanAirCombat)
				{
					GetCharacterMovement()->GravityScale = 0.0f;
					GetCharacterMovement()->Mass = 0;
					GetCharacterMovement()->StopMovementImmediately();
					CanAirCombat = false;
					//jumping = false;
				}
				else
				{
					GetCharacterMovement()->GravityScale = 2.0f;
					GetCharacterMovement()->Mass = 100;

				}

				FTimerHandle TimerHandle, TimerHandle2;
				float TimelineLength;
				switch (ComboStatus)
				{
				case 0:
					ComboStatus = 1;
					CharacterState = ECharacterState::C_COMBO1;
					/*
					if (FacingRight)
					{
						this->GetCharacterMovement()->AddForce(FVector(10000000.0f, 0.0f, 0.0f));
					}
					else
					{
						this->GetCharacterMovement()->AddForce(FVector(-10000000.0f, 0.0f, 0.0f));
					}
					*/
					RiseVoiceAudioComponent->SetSound(Attack1Sound);
					RiseVoiceAudioComponent->Play();

					TimelineLength = Weapon1Animation->GetNumFrames() / (float)Weapon1Animation->GetFramesPerSecond();
					GetWorldTimerManager().SetTimer(TimerHandle2, this, &AWandererCharacter::HandleAnimation, TimelineLength - 0.05f*TimelineLength, false);
					GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleCombat, TimelineLength - 0.40f*TimelineLength, false);


					InputManagementLevel = 1;
					HandleManagementLevel = 2;
					break;

				case 1:
					ComboStatus = 2;
					CharacterState = ECharacterState::C_COMBO2;

					RiseVoiceAudioComponent->SetSound(Attack2Sound);
					RiseVoiceAudioComponent->Play();

					TimelineLength = Weapon2Animation->GetNumFrames() / (float)Weapon2Animation->GetFramesPerSecond();
					GetWorldTimerManager().SetTimer(TimerHandle2, this, &AWandererCharacter::HandleAnimation, TimelineLength - 0.05f*TimelineLength, false);
					GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleCombat, TimelineLength - 0.40f*TimelineLength, false);
					HandleManagementLevel = 2;
					InputManagementLevel = 1;
					break;

				case 2:
					ComboStatus = 0;
					CharacterState = ECharacterState::C_COMBO3;

					RiseVoiceAudioComponent->SetSound(Attack3Sound);
					RiseVoiceAudioComponent->Play();

					TimelineLength = Weapon3Animation->GetNumFrames() / (float)Weapon3Animation->GetFramesPerSecond();
					GetWorldTimerManager().SetTimer(TimerHandle2, this, &AWandererCharacter::HandleAnimation, TimelineLength - 0.05f*TimelineLength, false);
					GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleCombat, TimelineLength - 0.40f*TimelineLength, false);
					HandleManagementLevel = 2;
					InputManagementLevel = 1;
					break;
				}
			}
			else // Upper Hit or Downward Hit
			{
				ComboStatus = 0;
				FTimerHandle TimerHandle;
				float TimelineLength;
				if (YAxisValue > 0)
				{
					CharacterState = ECharacterState::C_UPPERHIT;

					RiseVoiceAudioComponent->SetSound(Attack1Sound);
					RiseVoiceAudioComponent->Play();

					TimelineLength = UpperHitAnimation->GetNumFrames() / (float)UpperHitAnimation->GetFramesPerSecond();
					GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleUpperHit, TimelineLength - 0.05f*TimelineLength, false);
					HandleManagementLevel = 2;
					InputManagementLevel = 1;
					if (FacingRight)
					{
						Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
						SpriteDepth = 0.0f; // Needed for collision
						EnheduannaDepth = -2.0; // Needed for collision
						EffectDepth = -3.0f;
						GlowDepth = -1.0f;
						AlternateEnheduannaDepth = 1.0f;
						AlternateEffectDepth = -1.0f;
						AlternateGlowDepth = 2.0f;
					}
					else
					{
						Controller->SetControlRotation(FRotator(0.0, 0.0f, 0.0f));
						SpriteDepth = 0.0f; // Needed for collision
						EnheduannaDepth = 2.0; // Needed for collision
						EffectDepth = 3.0f;
						GlowDepth = 1.0f;
						AlternateEnheduannaDepth = -1.0f;
						AlternateEffectDepth = 1.0f;
						AlternateGlowDepth = -2.0f;
					}
					Super::Jump();
					jumping = true;
				}
				if (YAxisValue < 0)
				{

				}
			}
		}
	case 1:
		if (ComboStatus == 1 || ComboStatus == 2) { // We are already in combat mode, so this pressing means we want to continue
			KeepCombo = true;
		}
	case 0:
		break;
	}
}

void AWandererCharacter::HandleUpperHit()
{
	if (HandleManagementLevel >= 2)
	{
		if (FacingRight)
		{
			FacingRight = false;
		}
		else
		{
			FacingRight = true;
		}
		CharacterState = ECharacterState::C_JUMPFALL;
		jumping = true;
		InputManagementLevel = 2;
		HandleManagementLevel = 3;
		float TimelineLength = JumpFallAnimation->GetNumFrames() / (float)JumpFallAnimation->GetFramesPerSecond();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleJumpFallStatic, TimelineLength - 0.05f*TimelineLength, false);
	}
}

void AWandererCharacter::HandleAnimation()
{
	for (int i = 0; i < ParticleSystemArray.Num(); i++)
	{
		ParticleSystemArray[i]->DestroyComponent();
	}
	ParticleSystemArray.Empty();

	if (jumping)
	{
		GetCharacterMovement()->GravityScale = 2.0f;
		GetCharacterMovement()->Mass = 100.0f;
	}

	if (HandleManagementLevel >= 2)
	{
		if (KeepCombo && ComboStatus < 3)
		{
			if (jumping)
			{
				GetCharacterMovement()->GravityScale = 0.0f;
				GetCharacterMovement()->Mass = 0;
				GetCharacterMovement()->StopMovementImmediately();
			}
			else
			{
				GetCharacterMovement()->GravityScale = 2.0f;
				GetCharacterMovement()->Mass = 100;

			}

			if (ComboStatus == 1)
			{


				EnheduannaAudioComponent->SetSound(AttackEnheduannaSound);
				EnheduannaAudioComponent->Play();
				CharacterState = ECharacterState::C_COMBO2;

				RiseVoiceAudioComponent->SetSound(Attack2Sound);
				RiseVoiceAudioComponent->Play();

				FTimerHandle TimerHandle, TimerHandle2;
				float TimelineLength;
				TimelineLength = Weapon2Animation->GetNumFrames() / (float)Weapon2Animation->GetFramesPerSecond();
				GetWorldTimerManager().SetTimer(TimerHandle2, this, &AWandererCharacter::HandleAnimation, TimelineLength - 0.05f*TimelineLength, false);
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleCombat, TimelineLength - 0.40f*TimelineLength, false);
			}
			if (ComboStatus == 2)
			{
				EnheduannaAudioComponent->SetSound(AttackEnheduannaSound);
				EnheduannaAudioComponent->Play();
				CharacterState = ECharacterState::C_COMBO3;

				RiseVoiceAudioComponent->SetSound(Attack3Sound);
				RiseVoiceAudioComponent->Play();

				FTimerHandle TimerHandle, TimerHandle2;
				float TimelineLength;
				TimelineLength = Weapon3Animation->GetNumFrames() / (float)Weapon3Animation->GetFramesPerSecond();
				GetWorldTimerManager().SetTimer(TimerHandle2, this, &AWandererCharacter::HandleAnimation, TimelineLength - 0.05f*TimelineLength, false);
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AWandererCharacter::HandleCombat, TimelineLength - 0.40f*TimelineLength, false);
			}
			ComboStatus++;
			if (ComboStatus == 3)
				ComboStatus = 0;
		}
		else
		{
			ComboStatus = 0;
			CharacterState = ECharacterState::C_IDLE;
			InputManagementLevel = 2;
			HandleManagementLevel = 3;
		}
		KeepCombo = false;
	}
}


void AWandererCharacter::SettingsMenu()
{
	if (InputManagementLevel == 2)
	{
		if (InputManagementLevel >= 1)
		{
			if (InputManagementLevel >= 0)
			{
				AWandererHUD* wandererHUD = Cast<AWandererHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
				wandererHUD->changeToMenu();
			}
		}
	}
}

void AWandererCharacter::MoveRight(float Value)
{
	if (InputManagementLevel == 2)
	{
		//if (Value > 0) FacingRight = true;
		//if (Value < 0) FacingRight = false;

		if (Value > 0.6f || Value < -0.6f) Run = true;
		else Run = false;

		const FVector PlayerVelocity = GetVelocity();
		const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

		/*UpdateChar();*/
		if (Value < 0.0f && PlayerSpeedSqr > 0.0f) { // We are looking left and actually moving
													 // Change the camera boom target offset
			CameraBoom->TargetOffset = FVector(-200.0f, 0.0f, 0.0f);
			CameraBoom->CameraLagSpeed = 7.0f;
		}
		else {
			if (Value > 0.0f && PlayerSpeedSqr > 0.0f) { // We are looking right and actually moving
														 // Change the camera boom target offset
				CameraBoom->TargetOffset = FVector(200.0f, 0.0f, 0.0f);
				CameraBoom->CameraLagSpeed = 7.0f;
			}
		}

		// Apply the input to the character motion
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);

		if (InputManagementLevel >= 1)
		{
			XAxisValue = Value;

			if (InputManagementLevel >= 0)
			{

			}
		}
	}
}

/** Heavy logic of the character state should occur here */
void AWandererCharacter::UpdateCharacter()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(HandleManagementLevel));

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;

	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	if (this->GetActorRotation().Yaw != 0.0f)
	{
		FacingRight = false;
	} 
	else {
		FacingRight = true;
	}

	// Are we moving or standing still?
	if (CharacterState == ECharacterState::C_TALK || CharacterState == ECharacterState::C_RUN || CharacterState == ECharacterState::C_IDLE) {
		if (PlayerSpeedSqr > 0.0f)
		{
			CharacterState = ECharacterState::C_RUN;
		}
		else {
			CharacterState = ECharacterState::C_IDLE;
		}
	}

	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr && InputManagementLevel == 2)
	{
		if (XAxisValue < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
			SpriteDepth = 0.0f; // Needed for collision
			EnheduannaDepth = -2.0; // Needed for collision
			EffectDepth = -3.0f;
			GlowDepth = -1.0f;
			AlternateEnheduannaDepth = 1.0f;
			AlternateEffectDepth = -1.0f;
			AlternateGlowDepth = 2.0f;
		}
		else if (XAxisValue > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
			SpriteDepth = 0.0f; // Needed for collision
			EnheduannaDepth = 2.0; // Needed for collision
			EffectDepth = 3.0f;
			GlowDepth = 1.0f;
			AlternateEnheduannaDepth = -1.0f;
			AlternateEffectDepth = 1.0f;
			AlternateGlowDepth = -2.0f;
		}
	}
}
