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
	StopMovementTimer -= DeltaSeconds;

	if (auto pwn2 = Cast<ARobot>(GetPawn()))
	{
		if (pwn2->HasMissile)
		{
			Aggroed = true;
		}
	}

	if (NextScanCharge >= 1)
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

				if (auto pwn = Cast<ARobot>(GetPawn()))
				{
					if (pwn->HasMissile)
					{
						StopMovement();
						FRotator rot = FRotator(0, FMath::RadiansToDegrees(FMath::Atan2(pc->GetActorLocation().Y - pwn->GetActorLocation().Y, pc->GetActorLocation().X - pwn->GetActorLocation().X)), 0);
						pwn->SetActorRotation(rot);
						pwn->TryFireMissile = true;
						SetControlRotation(rot);
						StopMovementTimer = 2;
					}
				}
			}
		}

		NextScanCharge = 0;
	}

	if (Aggroed && pc && StopMovementTimer <= 0)
	{
		MoveToLocation(pc->GetActorLocation(), 80);

		if (auto pwn = Cast<ARobot>(GetPawn()))
		{
			pwn->TryPunch = true;
		}
	}
}
