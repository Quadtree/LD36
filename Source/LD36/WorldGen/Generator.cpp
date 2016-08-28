// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Generator.h"
#include "Actor/RobotRecycler.h"
#include "Robot.h"
#include "Actor/Prop.h"
#include "Actor/Dialog.h"

// Sets default values
AGenerator::AGenerator(const FObjectInitializer& oi)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorTiles = oi.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, "FloorTiles");
	RootComponent = FloorTiles;
}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	const int32 gridSizeX = GridSize;
	const int32 gridSizeY = GridSize;

	UE_LOG(LogTemp, Display, TEXT("gridSizeX=%s gridSizeY=%s"), *FString::FromInt(gridSizeX), *FString::FromInt(gridSizeY));

	TArray<TArray<int32>> roomIds;

	TArray<TArray<bool>> xDoor;
	TArray<TArray<bool>> yDoor;

	TArray<TArray<bool>> open;
	TArray<TArray<bool>> closed;

	TArray<int32> roomTypeMapping;
	

	for (int32 x = 0; x < gridSizeX; ++x) {
		roomIds.Add(TArray<int32>());
		xDoor.Add(TArray<bool>());
		yDoor.Add(TArray<bool>());
		open.Add(TArray<bool>());
		closed.Add(TArray<bool>());
		for (int32 y = 0; y < gridSizeY; ++y) {
			roomIds[x].Add(-1);
			xDoor[x].Add(false);
			yDoor[x].Add(false);
			open[x].Add(false);
			closed[x].Add(false);
		}
	}

	int32 totalTilesPlaced = 0;
	int32 totalTilesNeeded = gridSizeX*gridSizeY;
	int32 nextRoomId = 1;
	int32 maxItr = 0;

	TryPlaceRoom(GridSize / 2 - 2, GridSize / 2 - 2, GridSize / 2 + 2, GridSize / 2 + 2, nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds, roomTypeMapping, 2);

	int32 pStartRoomX = 3 + FMath::RandRange(0, 1) * (GridSize - 6);
	int32 pStartRoomY = 3 + FMath::RandRange(0, 1) * (GridSize - 6);

	TryPlaceRoom(pStartRoomX - 3, pStartRoomY - 3, pStartRoomX + 3, pStartRoomY + 3, nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds, roomTypeMapping, 1);
	FVector startRoomPos = GetActorLocation() + FVector(TileSize * (pStartRoomX - GridSize / 2), TileSize * (pStartRoomY - GridSize / 2), 100);

	for (int32 x = pStartRoomX; x >= 0; --x) {
		if (roomIds[x][pStartRoomY] != 2) {
			roomIds[x][pStartRoomY] = 2;
			totalTilesPlaced++;
		}
	}

	for (TActorIterator<APawn> p(GetWorld()); p; ++p)
	{
		p->SetActorLocation(startRoomPos + FVector(200, 0, 300));
	}

	auto a = GetWorld()->SpawnActor<AActor>(RecyclerType, startRoomPos, FRotator(0,180,0));
	a->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));

	while (totalTilesPlaced < totalTilesNeeded / 10)
	{
		int32 centerX = FMath::RandRange(0, gridSizeX - 1);
		int32 centerY = FMath::RandRange(0, gridSizeX - 1);

		if (FMath::RandBool())
		{
			TryPlaceRoom(centerX - FMath::RandRange(5, 20), centerY - FMath::RandRange(0, 1), centerX + FMath::RandRange(5, 20), centerY + FMath::RandRange(0, 1), nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds, roomTypeMapping, 0);
		}
		else
		{
			TryPlaceRoom(centerX - FMath::RandRange(0, 1), centerY - FMath::RandRange(5, 20), centerX + FMath::RandRange(0, 1), centerY + FMath::RandRange(5, 20), nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds, roomTypeMapping, 0);
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

		int32 roomType = -1;

		roomType = FMath::RandRange(3, RoomTypes.Num() - 1);

		TryPlaceRoom(centerX - FMath::RandRange(0, 7), centerY - FMath::RandRange(0, 7), centerX + FMath::RandRange(0, 7), centerY + FMath::RandRange(0, 7), nextRoomId, totalTilesPlaced, gridSizeX, gridSizeY, roomIds, roomTypeMapping, roomType);

		if (++maxItr > 100000) {
			UE_LOG(LogTemp, Warning, TEXT("Too many iterations! Abort!"));
			break;
		}
	}

	// door phase
	open[0][0] = true;

	TArray<FIntVector> deltas;
	deltas.Add(FIntVector(1, 0, 0));
	deltas.Add(FIntVector(0, 1, 0));
	deltas.Add(FIntVector(-1, 0, 0));
	deltas.Add(FIntVector(0, -1, 0));

	maxItr = 0;
	while (1)
	{
		bool anyOpen = true;
		while (anyOpen) {
			anyOpen = false;
			for (int32 x = 0; x < GridSize; ++x) {
				for (int32 y = 0; y < GridSize; ++y) {
					if (open[x][y]) {
						for (int32 di = 0; di < deltas.Num(); ++di) {
							auto& delta = deltas[di];

							int32 nx = x + delta.X;
							int32 ny = y + delta.Y;

							if (nx < 0 || ny < 0 || nx >= GridSize || ny >= GridSize) continue;

							if (roomIds[x][y] == roomIds[nx][ny] ||
								(di == 0 && xDoor[x][y]) ||
								(di == 1 && yDoor[x][y]) ||
								(di == 2 && x > 0 && xDoor[x - 1][y]) ||
								(di == 3 && y > 0 && yDoor[x][y - 1]))
							{
								if (!closed[nx][ny]) open[nx][ny] = true;
							}
						}

						closed[x][y] = true;
						open[x][y] = false;
						anyOpen = true;

						//UE_LOG(LogTemp, Display, TEXT("Closed %s,%s"), *FString::FromInt(x), *FString::FromInt(y));
					}
				}
			}
		}

		// now we've filled in all open squares
		bool done = true;
		for (int32 x = 0; x < GridSize; ++x) {
			for (int32 y = 0; y < GridSize; ++y) {
				if (!closed[x][y]) done = false;
			}
		}

		if (done) break;

		int32 ndx = -1, ndy = -1, n = 0;
		bool isXDoor = false;

		for (int32 x = 0; x < GridSize - 1; ++x) {
			for (int32 y = 0; y < GridSize - 1; ++y) {
				if (closed[x][y] != closed[x + 1][y] && FMath::RandRange(0, n++) == 0)
				{
					ndx = x;
					ndy = y;
					isXDoor = true;
				}
				if (closed[x][y] != closed[x][y + 1] && FMath::RandRange(0, n++) == 0)
				{
					ndx = x;
					ndy = y;
					isXDoor = false;
				}
			}
		}

		if (ndx > -1)
		{
			if (isXDoor)
			{
				UE_LOG(LogTemp, Display, TEXT("Adding X Door at %s,%s"), *FString::FromInt(ndx), *FString::FromInt(ndy));
				xDoor[ndx][ndy] = true;
				open[ndx + 1][ndy] = true;
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Adding Y Door at %s,%s"), *FString::FromInt(ndx), *FString::FromInt(ndy));
				yDoor[ndx][ndy] = true;
				open[ndx][ndy + 1] = true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unexpected lack of door creation?!"));
		}

		if (++maxItr > 1000) {
			UE_LOG(LogTemp, Warning, TEXT("Too many iterations in door creation! Abort!"));
			break;
		}
	}

	for (int32 x = 0; x < gridSizeX; ++x) {
		for (int32 y = 0; y < gridSizeY; ++y) {
			//DrawDebugString(GetWorld(), GetActorLocation() + FVector(TileSize * (x - GridSize / 2), TileSize * (y - GridSize / 2), 400), *FString::FromInt(roomIds[x][y]));
		}
	}

	TMap<UMaterial*, UInstancedStaticMeshComponent*> ismcMap;

	for (int32 x = -1; x < gridSizeX; ++x) {
		for (int32 y = -1; y < gridSizeY; ++y) {
			FVector tilePos = GetActorLocation() + FVector(TileSize * (x - GridSize / 2), TileSize * (y - GridSize / 2), 0);
			if (x >= 0 && y >= 0)
			{
				int32 roomId = roomIds[x][y];
				int32 roomType = 0;
				if (roomId != -1) roomType = roomTypeMapping[roomId];

				UMaterial* mat = RoomTypes[roomType].FloorMaterial;

				if (!ismcMap.Contains(mat))
				{
					auto ismc = NewObject<UInstancedStaticMeshComponent>(this);
					ismc->SetStaticMesh(FloorTiles->StaticMesh);
					ismc->SetMaterial(0, mat);
					ismc->AttachTo(RootComponent);
					ismc->RegisterComponent();
					FloorTileComponents.Add(ismc);
					ismcMap.Add(mat, ismc);
				}

				ismcMap[mat]->AddInstance(FTransform(FRotator(0, 0, 0), tilePos, FVector(1, 1, 1) * (TileSize / 100)));
			}

			FActorSpawnParameters params;
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			TArray<AActor*> nws;

			if ((y >= 0 && x >= 0 && x < gridSizeX - 1 && roomIds[x][y] != roomIds[x + 1][y]) || (x == -1 && y != -1) || (x == gridSizeX - 1 && y != -1))
			{
				if (x == -1 || y == -1 || !xDoor[x][y])
				{
					if (x == -1 || x == gridSizeX - 1)
						nws.Add(GetWorld()->SpawnActor<AActor>(PerimeterWallType, tilePos + FVector(TileSize / 2, 0, 100), FRotator(0, 0, 0), params));
					else
						nws.Add(GetWorld()->SpawnActor<AActor>(WallType, tilePos + FVector(TileSize / 2, 0, 100), FRotator(0, 0, 0), params));

					if (x == -1 && y == pStartRoomY)
					{
						nws[nws.Num() - 1]->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, ForceFieldMaterial);

						if (auto prop = Cast<AProp>(nws[nws.Num() - 1])) prop->Health = 9999999;
					}
				}
				else
				{
					// ...
				}
			}

			if ((y >= 0 && x >= 0 && y < gridSizeY - 1 && roomIds[x][y] != roomIds[x][y + 1]) || (y == -1 && x != -1) || (y == gridSizeY - 1 && x != -1))
			{
				if (x == -1 || y == -1 || !yDoor[x][y])
				{
					if (y == -1 || y == gridSizeY - 1)
						nws.Add(GetWorld()->SpawnActor<AActor>(PerimeterWallType, tilePos + FVector(0, TileSize / 2, 100), FRotator(0, 90, 0), params));
					else
						nws.Add(GetWorld()->SpawnActor<AActor>(WallType, tilePos + FVector(0, TileSize / 2, 100), FRotator(0, 90, 0), params));
				}
				else
				{
					// ...
				}
			}

			for (auto a : nws) {
				if (a) a->SetActorScale3D(FVector(1, TileSize / 100, 1));
			}
		}
	}

	maxItr = 0;

	for (int32 roomId = 1; roomId < roomTypeMapping.Num(); ++roomId)
	{
		if (RoomTypes[roomTypeMapping[roomId]].PropTypes.Num() == 0) continue;

		int32 totalTilesInRoom = 0;
		for (int32 x = 0; x < gridSizeX; ++x) {
			for (int32 y = 0; y < gridSizeY; ++y) {
				if (roomIds[x][y] == roomId) totalTilesInRoom++;
			}
		}

		int32 neededProps = (int32)(totalTilesInRoom * RoomTypes[roomTypeMapping[roomId]].PropDensity);
		int32 propsPlaced = 0;

		while (propsPlaced < neededProps)
		{
			for (int32 x = 0; x < gridSizeX; ++x) {
				for (int32 y = 0; y < gridSizeY; ++y) {
					if (roomIds[x][y] == roomId)
					{
						int32 otherAdjRooms = 0;

						if (x == 0 || roomIds[x - 1][y] != roomIds[x][y]) otherAdjRooms++;
						if (y == 0 || roomIds[x][y - 1] != roomIds[x][y]) otherAdjRooms++;
						if (x == gridSizeX - 1 || roomIds[x + 1][y] != roomIds[x][y]) otherAdjRooms++;
						if (y == gridSizeY - 1 || roomIds[x][y + 1] != roomIds[x][y]) otherAdjRooms++;

						if (FMath::RandRange(0, 15) <= otherAdjRooms)
						{
							FVector tilePos = GetActorLocation() + FVector(TileSize * (x - GridSize / 2), TileSize * (y - GridSize / 2), 200);
							FActorSpawnParameters params;
							params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
							AActor* na = nullptr;

							if (FMath::RandRange(0, 5) == 0) {
								na = GetWorld()->SpawnActor<ARobot>(NormalEnemySpawns[FMath::RandRange(0, NormalEnemySpawns.Num() - 1)], tilePos, FRotator::ZeroRotator);
								if (na) Cast<ARobot>(na)->SpawnDefaultController();
							}
							else
								na = GetWorld()->SpawnActor<AActor>(RoomTypes[roomTypeMapping[roomId]].PropTypes[FMath::RandRange(0, RoomTypes[roomTypeMapping[roomId]].PropTypes.Num() - 1)], tilePos, FRotator::ZeroRotator, params);

							if (na) propsPlaced++;
						}
					}
				}
			}

			if (++maxItr > 10000) break;
		}
		
	}

	for (auto a : FloorTileComponents)
	{
		// force bounds re-evaluation. bug workaround
		a->SetRelativeScale3D(FVector(0.9999f, 0.9999f, 0.9999f));
		a->SetRelativeScale3D(FVector(1,1,1));
	}

	GetWorld()->GetNavigationSystem()->ReleaseInitialBuildingLock();
}

// Called every frame
void AGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	TArray<FOverlapResult> res;
	FVector tilePos = GetActorLocation();

	if (GetWorld()->OverlapMultiByObjectType(res, tilePos, FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeBox(FVector(200, 200, 200))))
	{
		for (auto a : res)
		{
			if (auto rbt = Cast<ARobot>(a.Actor.Get()))
			{
				if (auto pc = Cast<APlayerController>(rbt->GetController()))
				{
					if (!HasEnteredComputerRoom)
					{
						UE_LOG(LogTemp, Display, TEXT("PC has entered main computer room"));
						HasEnteredComputerRoom = true;

						FVector tilePos = GetActorLocation() + FVector(TileSize * (FMath::RandRange(0, GridSize - 1) - GridSize / 2), TileSize * (FMath::RandRange(0, GridSize - 1) - GridSize / 2), 200);
						GetWorld()->SpawnActor<ARobot>(EliteEnemySpawns[FMath::RandRange(0, EliteEnemySpawns.Num() - 1)], tilePos, FRotator::ZeroRotator)->SpawnDefaultController();

						TArray<bool> isPC;
						isPC.Add(true);
						isPC.Add(false);
						isPC.Add(false);
						isPC.Add(true);
						isPC.Add(false);

						for (int32 i=0;i<isPC.Num();++i)
						{
							auto dialog = GetWorld()->SpawnActor<ADialog>();

							if (dialog)
							{
								if (isPC[i])
								{
									for (TActorIterator<ARobot> itr(GetWorld()); itr; ++itr)
									{
										if (Cast<APlayerController>(itr->GetController()))
										{
											dialog->AttachToActor(*itr, FAttachmentTransformRules::KeepRelativeTransform);
										}
									}
								}
								else
								{
									AProp* trg = nullptr;
									int32 n = 0;

									for (TActorIterator<AProp> itr(GetWorld()); itr; ++itr)
									{
										if (itr->IsMainComputer && FMath::RandRange(0, n++) == 0)
										{
											trg = *itr;
										}
									}

									if (trg) dialog->AttachToActor(trg, FAttachmentTransformRules::KeepRelativeTransform);
								}

								dialog->Duration = 4;
								dialog->SetKey(FName(*(FString(TEXT("mc_cnv_")) + FString::FromInt(i))));
								dialog->StartDelay = 4 * i;
							}
						}
					}
				}
			}
		}
	}
}

void AGenerator::TryPlaceRoom(int32 x1, int32 y1, int32 x2, int32 y2, int32& nextRoomId, int32& totalTilesPlaced, const int32& gridSizeX, const int32& gridSizeY, TArray<TArray<int32>>& roomIds, TArray<int32>& roomTypeMappings, int32 roomType)
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

		while (roomTypeMappings.Num() < nextRoomId + 1) roomTypeMappings.Add(0);
		roomTypeMappings[nextRoomId] = roomType;

		nextRoomId++;
	}
}

