// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "DialogHUD.generated.h"

/**
 * 
 */
UCLASS()
class LD36_API ADialogHUD : public AHUD
{
	GENERATED_BODY()

public:
	void DrawHUD();
	
	UPROPERTY(EditAnywhere, Category = Font)
	class UFont* DialogFont;
};
