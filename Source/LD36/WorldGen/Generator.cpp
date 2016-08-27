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

	const int32 gridSizeX = boxExtent.X / TileSize;
	const int32 gridSizeY = boxExtent.Y / TileSize;

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
}

// Called every frame
void AGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AGenerator::TryPlaceRoom(int32 x1, int32 x2, int32 y1, int32 y2, int32 & nextRoomId, int32 & totalTilesPlaced)
{
}

