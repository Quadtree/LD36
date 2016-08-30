// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "RobotRecycler.h"
#include "Robot.h"


// Sets default values
ARobotRecycler::ARobotRecycler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARobotRecycler::BeginPlay()
{
	Super::BeginPlay();
	SpawnCharge = 1000;
	ShouldSpawn = true;
}

// Called every frame
void ARobotRecycler::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	for (auto a : GetComponentsByTag(UPrimitiveComponent::StaticClass(), "Conveyor"))
	{
		if (auto p = Cast<UPrimitiveComponent>(a))
		{
			TArray<FOverlapResult> res;

			if (GetWorld()->OverlapMultiByObjectType(res, p->GetComponentLocation() + FVector(0,0,50), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeBox(p->GetComponentScale() * FVector(50, 50, 50))))
			{
				for (auto o : res)
				{
					if (o.Actor.Get() == this) continue;
					//UE_LOG(LogTemp, Display, TEXT("Conveying=%s"), (o.Actor.IsValid() ? *o.Actor->GetName() : TEXT("???")));

					//if (o.Component.IsValid())
					//{
						//o.Component->AddImpulse(FVector(-50, 0, 0), NAME_None, true);

					//}
					if (o.Actor.IsValid())
					{
						o.Actor->SetActorLocation(o.Actor->GetActorLocation() + FVector(25, 0, 0) * DeltaTime);
					}
				}
			}
		}
	}
	TArray<FOverlapResult> res2;

	if (GetWorld()->OverlapMultiByObjectType(res2, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(50)))
	{
		for (auto a2 : res2)
		{
			if (a2.Actor.IsValid())
			{
				a2.Actor->TakeDamage(10000, FDamageEvent(), nullptr, nullptr);

				auto rbt = Cast<ARobot>(a2.Actor.Get());

				if (rbt)
				{
					if (!Cast<APlayerController>(rbt->GetController())) rbt->Destroy();
				}
			}
		}
	}

	SpawnCharge += DeltaTime;

	if (SpawnCharge >= 3 && ShouldSpawn) {
		GetWorld()->SpawnActor<ARobot>(PlayerRobotType, GetActorLocation() + FVector(-500, 0, 120), FRotator::ZeroRotator)->SpawnDefaultController();
		SpawnCharge = 0;
	}
}

