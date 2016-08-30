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

	NextScanCharge += DeltaSeconds;
	StopMovementTimer -= DeltaSeconds;

	auto pwn2 = Cast<ARobot>(GetPawn());

	if (pwn2)
	{
		if (pwn2->HasMissile)
		{
			Aggroed = true;
		}
	}

	if (NextScanCharge >= (Aggroed ? 0.33f : 1.0f))
	{
		APawn* pc = nullptr;

		for (TActorIterator<APawn> i(GetPawn()->GetWorld()); i; ++i)
		{
			if (auto controller = Cast<APlayerController>(i->GetController()))
			{
				pc = *i;
			}
		}

		if (pc)
		{
			FCollisionQueryParams params;
			params.AddIgnoredActor(GetPawn());
			params.AddIgnoredActor(pc);
			if ((!Aggroed || pwn2->HasMissile) && !GetPawn()->GetWorld()->LineTraceTestByObjectType(GetPawn()->GetActorLocation() + FVector(0, 0, 50), pc->GetActorLocation() + FVector(0,0,50), FCollisionObjectQueryParams::AllObjects, params))
			{
				Aggroed = true;
				UE_LOG(LogTemp, Display, TEXT("%s Aggroed"), *GetName());

				if (pwn2->HasMissile)
				{
					StopMovement();
					FRotator rot = FRotator(0, FMath::RadiansToDegrees(FMath::Atan2(pc->GetActorLocation().Y - pwn2->GetActorLocation().Y, pc->GetActorLocation().X - pwn2->GetActorLocation().X)), 0);
					pwn2->SetActorRotation(rot);
					pwn2->TryFireMissile = true;
					SetControlRotation(rot);
					StopMovementTimer = 2;

					if (FMath::RandRange(0, 5) == 0)
					{
						pwn2->Say(*(FString(TEXT("killbot")) + FString::FromInt(FMath::RandRange(0,3))));
					}
				}

				if (auto pcp = Cast<ARobot>(pc))
				{
					if (!pcp->SDF_SawMissile && pwn2->HasMissile)
					{
						pcp->Say(TEXT("see_missile"));
						pcp->SDF_SawMissile = true;
					}
					if (!pcp->SDF_SawMace && pwn2->HasMace)
					{
						pcp->Say(TEXT("see_mace"));
						pcp->SDF_SawMace = true;
					}
				}
			}
		}

		if (Aggroed && pc && StopMovementTimer <= 0)
		{
			MoveToLocation(pc->GetActorLocation(), 80);

			if (auto pwn = Cast<ARobot>(GetPawn()))
			{
				pwn->TryPunch = false;
				if (FVector::DistSquared(pwn->GetActorLocation(), pc->GetActorLocation()) < FMath::Square(200)) pwn->TryPunch = true;
			}
		}

		if (Aggroed && pc)
		{
			if (auto pwn = Cast<ARobot>(pc))
			{
				if (!pwn->SDF_FirstContact)
				{
					pwn->Say(TEXT("contact"));
					pwn->SDF_FirstContact = true;
				}
			}
		}

		NextScanCharge = 0;
	}
}
