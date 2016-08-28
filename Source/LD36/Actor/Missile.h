// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UCLASS()
class LD36_API AMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = Missile)
	class UParticleSystem* SmokeTrail;
	
	UPROPERTY(EditAnywhere, Category = Missile)
	class UParticleSystem* Explosion;

	UPROPERTY(EditAnywhere, Category = Missile)
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, Category = Missile)
	float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	class USoundBase* DeathSound;
};
