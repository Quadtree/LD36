// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "RobotMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class LD36_API URobotMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	virtual bool ShouldSkipUpdate(float DeltaTime) const override;
	
private:
	bool IsAggroed() const;
};
