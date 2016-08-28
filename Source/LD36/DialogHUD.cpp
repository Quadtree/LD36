// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "DialogHUD.h"
#include "Actor/Dialog.h"

void ADialogHUD::DrawHUD()
{
	Super::DrawHUD();

	TArray<FVector> centers;

	for (TActorIterator<ADialog> i(GetWorld()); i; ++i)
	{
		if (!i->GetText().IsEmpty())
		{
			FVector pos = Project(i->GetActorLocation() + FVector(0,0,150));

			for (auto center : centers)
			{
				if (FVector::DistSquaredXY(center, pos) < FMath::Square(200))
				{
					pos.Y += 50;
				}
			}

			centers.Add(pos);

			if (pos.Z > 0 && pos.X >= -200 && pos.Y >= -200 && pos.X <= Canvas->ClipX + 200 && pos.Y <= Canvas->ClipY + 200)
			{
				FCanvasTextItem txt(FVector2D(pos.X, pos.Y), i->GetText(), DialogFont, FColor::White);
				txt.bCentreX = true;
				Canvas->DrawItem(txt);
			}
		}
	}
}


