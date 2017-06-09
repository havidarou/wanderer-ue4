// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Shot.generated.h"

UCLASS()
class WANDERER_API AShot : public AActor
{
	GENERATED_BODY()
	
protected:

	// The animation to play when shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	class USphereComponent* CollisionComponent;

	// The animation to play when shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovement;

	// The animation to play when shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbookComponent* ShootingComponent;

	// The animation to play when shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* ShootingAnimation;

public:	

	UFUNCTION()
	void InflictDamage(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Sets default values for this actor's properties
	AShot();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
