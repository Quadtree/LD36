// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoomType.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FRoomType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Room)
	float PropDensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Room)
	TArray<TSubclassOf<AActor>> PropTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Room)
	UMaterial* FloorMaterial;
};
