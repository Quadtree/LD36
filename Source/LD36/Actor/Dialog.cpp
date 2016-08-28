// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Dialog.h"
#include "DialogTableRow.h"


// Sets default values
ADialog::ADialog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> dt(TEXT("/Game/Data/DialogTable"));
	StringTable = dt.Object;
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

