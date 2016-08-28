// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DialogTableRow.generated.h"

/**
 * 
 */
USTRUCT()
struct LD36_API FDialogTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Text)
	FString Text;
	
	
};
