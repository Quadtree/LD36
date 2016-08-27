// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseWall.generated.h"

/**
 * 
 */
UCLASS()
class LD36_API ABaseWall : public AActor
{
	GENERATED_BODY()
public:
	ABaseWall();

	virtual void Tick(float DeltaSeconds) override;
	
	
};
