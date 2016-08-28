// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RobotRecycler.generated.h"

UCLASS()
class LD36_API ARobotRecycler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARobotRecycler();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = WorldGen)
	TSubclassOf<class ARobot> PlayerRobotType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldGen)
	bool ShouldSpawn;

private:
	float SpawnCharge;
	
};
