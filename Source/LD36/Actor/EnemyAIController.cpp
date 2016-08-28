// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "EnemyAIController.h"
#include "Robot.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!GetPawn()) return;

	APawn* pc = nullptr;

	for (TActorIterator<APawn> i(GetPawn()->GetWorld()); i; ++i)
	{
		if (auto controller = Cast<APlayerController>(i->GetController()))
		{
			pc = *i;
		}
	}

	NextScanCharge += DeltaSeconds;

	if (NextScanCharge >= 1 && !Aggroed)
	{
		if (pc)
		{
			FCollisionQueryParams params;
			params.AddIgnoredActor(GetPawn());
			params.AddIgnoredActor(pc);
			if (GetPawn()->GetWorld()->LineTraceTestByChannel(GetPawn()->GetActorLocation() + FVector(0, 0, 200), FVector(0, 0, 0), ECollisionChannel::ECC_Visibility, params))
			{
				Aggroed = true;
				UE_LOG(LogTemp, Display, TEXT("%s Aggroed"), *GetName());
			}
		}

		NextScanCharge = 0;
	}

	if (Aggroed && pc)
	{
		MoveToLocation(pc->GetActorLocation(), 80);

		if (auto pwn = Cast<ARobot>(GetPawn()))
		{
			pwn->TryPunch = true;
		}
	}
}
