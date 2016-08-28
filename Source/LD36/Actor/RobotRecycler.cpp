// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "RobotRecycler.h"


// Sets default values
ARobotRecycler::ARobotRecycler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARobotRecycler::BeginPlay()
{
	Super::BeginPlay();
	
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
}

