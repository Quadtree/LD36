// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Dialog.h"


// Sets default values
ADialog::ADialog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADialog::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADialog::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FText ADialog::GetText()
{
	return FText::FromString(TEXT("???"));
}

