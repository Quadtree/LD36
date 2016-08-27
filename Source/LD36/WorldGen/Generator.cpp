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
}

// Called every frame
void AGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

