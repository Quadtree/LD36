// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Robot.generated.h"

UCLASS()
class LD36_API ARobot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARobot();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
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

	float PunchLockoutTimer;
	float KickLockoutTimer;

	UPROPERTY(EditAnywhere, Category = Attack)
	FName FistBoneName;

	UPROPERTY(EditAnywhere, Category = Attack)
	FName FootBoneName;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, Category = Health)
	float Health;

	UPROPERTY(EditAnywhere, Category = Health)
	float MaxHealth;
	
private:
	bool ManualMovementMode;
	
	void SetMovementLeftRight(float value);
	void SetMovementUpDown(float value);

	void SetTryPunch(float value);
	void SetTryKick(float value);

	FVector2D ManualMovement;

	FTransform DefaultMeshRelativeTransform;

	float FallBlend;

	FVector OnGroundLoc;

	void MeleeAttack(const FName& boneName, float& lockoutTimer, float damage, float stunDamage, float minCoreDistance);

	void UpdateStandingStatus();
};
