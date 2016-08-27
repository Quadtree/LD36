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
	
private:
	bool ManualMovementMode;
	
	void SetMovementLeftRight(float value);
	void SetMovementUpDown(float value);

	FVector2D ManualMovement;
};
