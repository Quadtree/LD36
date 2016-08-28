// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "DialogHUD.h"
#include "Actor/Dialog.h"

void ADialogHUD::DrawHUD()
{
	Super::DrawHUD();

	for (TActorIterator<ADialog> i(GetWorld()); i; ++i)
	{
		FVector pos = Project(i->GetActorLocation());

		if (pos.Z > 0 && pos.X >= -200 && pos.Y >= -200 && pos.X <= Canvas->ClipX + 200 && pos.Y <= Canvas->ClipY + 200)
		{
			FCanvasTextItem txt(FVector2D(pos.X, pos.Y), i->GetText(), DialogFont, FColor::White);
			txt.bCentreX = true;
			Canvas->DrawItem(txt);
		}
	}
}


