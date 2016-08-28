// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Prop.h"
#include "DamageType/StunDamage.h"

float AProp::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (DamageEvent.DamageTypeClass != UStunDamage::StaticClass())
	{
		Health -= FMath::Max(DamageAmount, 0.f);
	}

	if (Health <= 0)
	{
		Destroy();
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
