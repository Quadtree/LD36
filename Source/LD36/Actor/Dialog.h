// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Dialog.generated.h"

UCLASS()
class LD36_API ADialog : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	FText GetText();
	
	UFUNCTION(BlueprintCallable, Category = Dialog)
	void SetKey(FName key);

	UPROPERTY(BlueprintReadWrite, Category = Dialog)
	float StartDelay;

	UPROPERTY(BlueprintReadWrite, Category = Dialog)
	float Duration;

	UPROPERTY(VisibleAnywhere, Category = Dialog)
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = Dialog)
	class UAudioComponent* AudioComponent;

	UPROPERTY()
	class UDataTable* StringTable;

	UPROPERTY()
	TArray<USoundBase*> Words;

private:
	UFUNCTION()
	void Update();

	FName Key;

	TArray<FString> SpeakQueue;

	bool IsPrimary;

	FTimerHandle UpdateTimerHandle;

	bool HasBeenSpedUp;
};
