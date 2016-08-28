// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Dialog.generated.h"

UCLASS()
class LD36_API ADialog : public AActor
{
	GENERATED_BODY()
	
public:	
	ADialog();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FText GetText();
	
	void SetKey(FName key);

	UPROPERTY()
	class UDataTable* StringTable;

private:
	FName Key;
};
