// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Dialog.h"
#include "DialogTableRow.h"


// Sets default values
ADialog::ADialog(const FObjectInitializer& oi)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> dt(TEXT("/Game/Data/DialogTable"));
	StringTable = dt.Object;

	SceneComponent = oi.CreateDefaultSubobject<USceneComponent>(this, "SceneComponent");
	RootComponent = SceneComponent;
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

	StartDelay -= DeltaTime;

	if (StartDelay <= 0)
	{
		Duration -= DeltaTime;
		if (Duration <= 0) Destroy();
	}

	//DrawDebugString(GetWorld(), GetActorLocation(), GetText().ToString(), nullptr, FColor::Green);
}

FText ADialog::GetText()
{
	if (StartDelay > 0)
	{
		return FText::FromString(TEXT("<WAIT>"));
	}

	FString ctx;
	FDialogTableRow* row = StringTable->FindRow<FDialogTableRow>(Key, ctx);

	if (row)
	{
		return FText::FromString(row->Text);
	}
	else
	{
		return FText::FromString(TEXT("???"));
	}

	//return FText::FromString(TEXT("???"));
}

void ADialog::SetKey(FName key)
{
	this->Key = key;
}

