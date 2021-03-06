// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Robot.generated.h"

UCLASS()
class LD36_API ARobot : public ACharacter
{
	GENERATED_UCLASS_BODY()

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type reason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	bool OnFeet;

	UPROPERTY(BlueprintReadWrite, Category = Stun)
	float StunTime;

	UPROPERTY(BlueprintReadOnly, Category = Attack)
	bool IsPunching;

	UPROPERTY(BlueprintReadOnly, Category = Attack)
	bool IsKicking;

	bool TryPunch;

	bool TryKick;

	bool TryFireMissile;

	float MissileCharge;

	float PunchLockoutTimer;
	float KickLockoutTimer;

	UPROPERTY(EditAnywhere, Category = Attack)
	FName FistBoneName;

	UPROPERTY(EditAnywhere, Category = Attack)
	FName FootBoneName;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Health)
	float Health;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Health)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Speed)
	float ActionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	bool HasMissile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	bool HasMace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> MissileType;

	UPROPERTY(EditAnywhere, Category = Missile)
	class UParticleSystem* Explosion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	class USoundBase* AttackHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	class USoundBase* MissileLaunchSound;

	bool SDF_FirstContact;
	bool SDF_GotMace;
	bool SDF_SawMace;
	bool SDF_GotMissile;
	bool SDF_SawMissile;
	bool SDF_SawFirstOil;

	bool Intact;

	void Say(FName key, float delay = 0);
	
private:
	bool ManualMovementMode;
	
	void SetMovementLeftRight(float value);
	void SetMovementUpDown(float value);

	void SetTryPunch(float value);
	void SetTryKick(float value);
	void SetTryFireMissile(float value);

	FVector2D ManualMovement;

	FTransform DefaultMeshRelativeTransform;

	float FallBlend;

	FVector OnGroundLoc;

	void MeleeAttack(const FName& boneName, float& lockoutTimer, float damage, float stunDamage, float minCoreDistance);

	void UpdateStandingStatus();

	UFUNCTION()
	void HalfSecondUpdate();

	FTimerHandle HalfSecondUpdateTimerHandle;

	AActor* Target;

	void AcquireTarget();
};
