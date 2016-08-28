// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RoomType.h"
#include "Generator.generated.h"

UCLASS()
class LD36_API AGenerator : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = WorldGen)
	float TileSize;

	UPROPERTY(EditAnywhere, Category = WorldGen)
	int32 GridSize;
	
	UPROPERTY(EditAnywhere, Category = WorldGen)
	class UInstancedStaticMeshComponent* FloorTiles;

	TArray<class UInstancedStaticMeshComponent*> FloorTileComponents;

	UPROPERTY(EditAnywhere, Category = WorldGen)
	TSubclassOf<AActor> WallType;

	UPROPERTY(EditAnywhere, Category = WorldGen)
	TSubclassOf<class ARobotRecycler> RecyclerType;

	// room types. assume 0 is corridor, 1 is starting room, 2 is central computer room
	UPROPERTY(EditAnywhere, Category = WorldGen)
	TArray<FRoomType> RoomTypes;

private:
	void TryPlaceRoom(int32 x1, int32 y1, int32 x2, int32 y2, int32& nextRoomId, int32& totalTilesPlaced, const int32& gridSizeX, const int32& gridSizeY, TArray<TArray<int32>>& roomIds, TArray<int32>& roomTypeMappings, int32 roomType);
};
