// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "LD36GameMode.h"

void ALD36GameMode::BeginPlay()
{
	Super::BeginPlay();

#if PLATFORM_HTML5
	GEngine->GameUserSettings->ScalabilityQuality.AntiAliasingQuality = 3;
	GEngine->GameUserSettings->ScalabilityQuality.EffectsQuality = 3;
	GEngine->GameUserSettings->ScalabilityQuality.PostProcessQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.ResolutionQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.TextureQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.ViewDistanceQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.ShadowQuality = 3;
	GEngine->GameUserSettings->ApplySettings(false);
#endif
}


