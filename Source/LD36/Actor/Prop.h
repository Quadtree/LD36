// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Prop.generated.h"

/**
 * 
 */
UCLASS()
class LD36_API AProp : public AActor
{
	GENERATED_BODY()
public:
	AProp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prop)
	float Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prop)
	float OnFirePct;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prop)
	float DetachPct;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prop)
	float BurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prop)
	TSubclassOf<AActor> DestructionDebris;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prop)
	float DebrisCount;

	UPROPERTY(EditAnywhere, Category = Missile)
	class UParticleSystem* Explosion;

	UPROPERTY(EditAnywhere, Category = Missile)
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, Category = Missile)
	float ExplosionDamage;

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Tick(float DeltaSeconds) override;
private:
	float MaxHealth;
};
