// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "RobotMovementComponent.h"
#include "Actor/EnemyAIController.h"

bool URobotMovementComponent::ShouldSkipUpdate(float DeltaTime) const
{
	return !IsAggroed() || Super::ShouldSkipUpdate(DeltaTime);
}

bool URobotMovementComponent::IsAggroed() const
{
	if (auto o = GetOwner()) if (auto pwn = Cast<APawn>(o)) if (auto aic = Cast<AEnemyAIController>(pwn->GetController()))
	{
		return aic->Aggroed;
	}
	
	return true;
}
