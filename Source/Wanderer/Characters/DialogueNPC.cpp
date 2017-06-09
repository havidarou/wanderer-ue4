// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "PaperFlipbookComponent.h"
#include "Components/WidgetComponent.h"
#include "Characters/DialogueNPC.h"

ADialogueNPC::ADialogueNPC()
{

	Health = 1.5f;
	currentHealth = Health;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
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

	bReplicates = true;
	
	// Loading the talk widget
	static ConstructorHelpers::FClassFinder<UUserWidget> TalkWidgetObj(TEXT("/Game/2DSideScrollerCPP/Blueprints/Dialogue/TalkWidget.TalkWidget_C"));
	if (TalkWidgetObj.Succeeded()) {
		TalkWidgetClass = TalkWidgetObj.Class;
	}
	else {
		// TalkWidgetObj not found
		TalkWidgetClass = nullptr;
	}
	
	TalkWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Talk Widget"));
	TalkWidgetComp->SetupAttachment(RootComponent);
	TalkWidgetComp->AddRelativeRotation(FQuat(FRotator(0.0f, 90.0f, 0.0f)));
	TalkWidgetComp->SetVisibility(true);
	TalkWidgetComp->SetWidgetClass(TalkWidgetClass);
	TalkWidgetComp->SetWorldScale3D(FVector(0.5f));
	TalkWidgetComp->SetWorldLocation(FVector(110.0f, 0.0f, 0.0f));
	
}

void ADialogueNPC::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ADialogueNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCharacter();
}

void ADialogueNPC::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;
}



