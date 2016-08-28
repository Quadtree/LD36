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
};
