// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Generator.generated.h"

UCLASS()
class LD36_API AGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = WorldGen)
	float TileSize;
	
	UPROPERTY(EditAnywhere, Category = WorldGen)
	TSubclassOf<AActor> FloorTileType;

	UPROPERTY(EditAnywhere, Category = WorldGen)
	TSubclassOf<AActor> WallType;

private:
	void TryPlaceRoom(int32 x1, int32 y1, int32 x2, int32 y2, int32& nextRoomId, int32& totalTilesPlaced);
};
