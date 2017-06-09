// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "PaperFlipbookComponent.h"
#include "WandererCharacter.h"
#include "Shot.h"


// Sets default values
AShot::AShot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The collision component for the shot
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComponent->InitSphereRadius(7.0f);
	RootComponent = CollisionComponent;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 800.f;
	ProjectileMovement->ProjectileGravityScale = 0; // No gravity, it travels in a straight line
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	ShootingComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ShootingAnimation"));
	ShootingComponent->SetupAttachment(RootComponent);
	ShootingComponent->SetIsReplicated(true);

	static ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> Shot(TEXT("PaperFlipbook'/Game/2DSideScroller/Textures/Enemies/Shot-Flipbook.Shot-Flipbook'"));
	ShootingAnimation = Shot.Get();
	ShootingComponent->SetFlipbook(ShootingAnimation);
	ShootingComponent->SetWorldScale3D(FVector(0.20f, 0.20f, 0.20f));

	ShootingComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ShootingComponent->bGenerateOverlapEvents = 1;

	ShootingComponent->OnComponentBeginOverlap.AddDynamic(this, &AShot::InflictDamage);
}

void AShot::InflictDamage(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;

	// We hit the right component in the WandererCharacter object
	if (Cast<AWandererCharacter>(OtherActor) && OtherComp->GetName() == "Sprite0")
	{
		// We aren't blocking
		if (Cast<AWandererCharacter>(OtherActor)->CharacterState != ECharacterState::C_BLOCK && Cast<AWandererCharacter>(OtherActor)->CharacterState != ECharacterState::C_BLOCKHOLD)
		{
			Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.15f, false, false);
			this->Destroy();
		}
		// We are blocking
		else
		{
			// If the shot is moving right and we are blocking facing left, then it's a good block
			if (Cast<AWandererCharacter>(OtherActor)->IsFacingRight() == false && TravelDirection > 0.0f)
			{
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCK) // Precise block
				{
					// Play fancy animation
					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
					this->Destroy();
				}
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCKHOLD) // Non precise block
				{

					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
					this->Destroy();
				}
			}
			else if (Cast<AWandererCharacter>(OtherActor)->IsFacingRight() == true && TravelDirection < 0.0f)
			{
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCK) // Precise block
				{
					// Play fancy animation
					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
					this->Destroy();
				}
				if (Cast<AWandererCharacter>(OtherActor)->CharacterState == ECharacterState::C_BLOCKHOLD) // Non precise block
				{
					Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.0f, false, false);
					this->Destroy();
				}
			}
			// We are blocking, but we are not facing correctly
			else
			{
				Cast<AWandererCharacter>(OtherActor)->ReceiveDamage(0.15f, false, false);
				this->Destroy();
			}
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "No es Enemigo");
	}
}

// Called when the game starts or when spawned
void AShot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShot::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

