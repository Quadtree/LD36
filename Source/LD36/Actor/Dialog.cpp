// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Dialog.h"
#include "DialogTableRow.h"
#include "Regex.h"


// Sets default values
ADialog::ADialog(const FObjectInitializer& oi)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> dt(TEXT("/Game/Data/DialogTable"));
	StringTable = dt.Object;

	SceneComponent = oi.CreateDefaultSubobject<USceneComponent>(this, "SceneComponent");
	RootComponent = SceneComponent;

	AudioComponent = oi.CreateDefaultSubobject<UAudioComponent>(this, "AudioComponent");
	AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	static TArray<ConstructorHelpers::FObjectFinder<USoundBase>> soundFinders;

	if (soundFinders.Num() == 0)
	{
		FRegexPattern pattern(TEXT("[a-z]+"));

		for (auto name : dt.Object->GetRowNames())
		{
			FString ctx;
			FRegexMatcher m(pattern, dt.Object->FindRow<FDialogTableRow>(name, ctx)->Text.ToLower());

			while (m.FindNext())
			{
				UE_LOG(LogTemp, Display, TEXT("word=%s"), *m.GetCaptureGroup(0));
				soundFinders.Add(ConstructorHelpers::FObjectFinder<USoundBase>(*(FString(TEXT("/Game/Sounds/Dialog/cleaned/")) + m.GetCaptureGroup(0))));
			}
		}
	}

	for (auto sf : soundFinders)
	{
		if (sf.Succeeded()) Words.Add(sf.Object);
	}
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

	if (!IsPrimary())
	{
		StartDelay = FMath::Max(StartDelay, 0.5f);
	}
	else if (StartDelay <= 0)
	{
		if (!AudioComponent->IsPlaying() && SpeakQueue.Num() > 0)
		{
			UE_LOG(LogTemp, Display, TEXT("Attempting to play %s"), *SpeakQueue[0]);
			for (auto sound : Words)
			{
				if (sound->GetName() == SpeakQueue[0])
				{
					AudioComponent->SetSound(sound);
					AudioComponent->Play();
					break;
				}
			}

			SpeakQueue.RemoveAt(0);
		}

		if (SpeakQueue.Num() > 0 || AudioComponent->IsPlaying())
			Duration = FMath::Max(Duration, 0.25f);
		else
			Duration = -1;
	}


	//DrawDebugString(GetWorld(), GetActorLocation(), GetText().ToString(), nullptr, FColor::Green);
}

FText ADialog::GetText()
{
	if (StartDelay > 0)
	{
		return FText::FromString(TEXT(""));
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

	FString ctx;
	FDialogTableRow* row = StringTable->FindRow<FDialogTableRow>(Key, ctx);

	if (row)
	{
		FRegexPattern pattern(TEXT("[a-z]+"));
		FRegexMatcher m(pattern, row->Text.ToLower());

		while (m.FindNext())
		{
			SpeakQueue.Add(m.GetCaptureGroup(0));
		}
	}
}

bool ADialog::IsPrimary()
{
	if (!RootComponent || !RootComponent->IsValidLowLevel()) return true;
	if (!RootComponent->GetAttachParent() || RootComponent->GetAttachParent()->IsValidLowLevel()) return true;

	TArray<USceneComponent*> comps;
	RootComponent->GetAttachParent()->GetChildrenComponents(false, comps);

	for (auto c : comps)
	{
		if (c && c->IsValidLowLevel() && Cast<ADialog>(c->GetOwner()))
		{
			if (c->GetOwner() != this)
				return false;
			else
				return true;
		}
	}

	return true;
}

