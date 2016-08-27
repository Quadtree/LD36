// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Generator.h"


// Sets default values
AGenerator::AGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	FVector origin, boxExtent;
	this->GetActorBounds(false, origin, boxExtent);

	UE_LOG(LogTemp, Display, TEXT("generator bounds=%s"), *boxExtent.ToString());

	const int32 gridSizeX = boxExtent.X / TileSize*2;
	const int32 gridSizeY = boxExtent.Y / TileSize*2;

	UE_LOG(LogTemp, Display, TEXT("gridSizeX=%s gridSizeY=%s"), *FString::FromInt(gridSizeX), *FString::FromInt(gridSizeY));

	TArray<TArray<int32>> roomIds;

	for (int32 x = 0; x < gridSizeX; ++x) {
		roomIds.Add(TArray<int32>());
		for (int32 y = 0; y < gridSizeY; ++y) {
			roomIds[x].Add(-1);
		}
	}

	int32 totalTilesPlaced = 0;
	int32 totalTilesNeeded = gridSizeX*gridSizeY;
	int32 nextRoomId = 1;
	int32 maxItr = 0;

	while (totalTilesPlaced < totalTilesNeeded / 10)
	{
		int32 centerX = FMath::RandRange(0, gridSizeX - 1);
		int32 centerY = FMath::RandRange(0, gridSizeX - 1);

		if (FMath::RandBool())
		{
			TryPlaceRoom(centerX - FMath::RandRange(5, 20), centerY - FMath::RandRange(0, 1), centerX + FMath::RandRange(5, 20), centerY + FMath::RandRange(0, 1), nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds);
		}
		else
		{
			TryPlaceRoom(centerX - FMath::RandRange(0, 1), centerY - FMath::RandRange(5, 20), centerX + FMath::RandRange(0, 1), centerY + FMath::RandRange(5, 20), nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds);
		}

		if (++maxItr > 100000) {
			UE_LOG(LogTemp, Warning, TEXT("Too many iterations! Abort!"));
			break;
		}
	}

	while (totalTilesPlaced < totalTilesNeeded)
	{
		int32 centerX = FMath::RandRange(0, gridSizeX - 1);
		int32 centerY = FMath::RandRange(0, gridSizeX - 1);

		TryPlaceRoom(centerX - FMath::RandRange(0, 7), centerY - FMath::RandRange(0, 7), centerX + FMath::RandRange(0, 7), centerY + FMath::RandRange(0, 7), nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds);

		if (++maxItr > 100000) {
			UE_LOG(LogTemp, Warning, TEXT("Too many iterations! Abort!"));
			break;
		}
	}

	for (int32 x = 0; x < gridSizeX; ++x) {
		for (int32 y = 0; y < gridSizeY; ++y) {
			DrawDebugString(GetWorld(), origin - boxExtent + FVector(TileSize * x, TileSize * y, 400), *FString::FromInt(roomIds[x][y]));
		}
	}
}

// Called every frame
void AGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AGenerator::TryPlaceRoom(int32 x1, int32 y1, int32 x2, int32 y2, int32& nextRoomId, int32& totalTilesPlaced, const int32& gridSizeX, const int32& gridSizeY, TArray<TArray<int32>>& roomIds)
{
	x1 = FMath::Min(x1, x2);
	y1 = FMath::Min(y1, y2);
	x2 = FMath::Max(x1, x2);
	y2 = FMath::Max(y1, y2);

	x1 = FMath::Clamp(x1, 0, gridSizeX - 1);
	x2 = FMath::Clamp(x2, 0, gridSizeX - 1);
	y1 = FMath::Clamp(y1, 0, gridSizeY - 1);
	y2 = FMath::Clamp(y2, 0, gridSizeY - 1);

	int32 tilesOpen = 0;

	for (int32 x = x1; x < x2; ++x) {
		for (int32 y = y1; y < y2; ++y) {
			if (roomIds[x][y] == -1) tilesOpen++;
		}
	}

	if (tilesOpen >= (x2 - x1 + 1)*(y2 - y1 + 1)/2)
	{
		for (int32 x = x1; x <= x2; ++x) {
			for (int32 y = y1; y <= y2; ++y) {
				if (roomIds[x][y] == -1) {
					roomIds[x][y] = nextRoomId;
					totalTilesPlaced++;
				}
			}
		}

		UE_LOG(LogTemp, Display, TEXT("New room placed %s, %s"), *FString::FromInt(nextRoomId), *FString::FromInt(totalTilesPlaced));

		nextRoomId++;
	}
}

