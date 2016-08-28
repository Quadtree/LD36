// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "StunDamage.h"

UStunDamage::UStunDamage()
{
	bCausedByWorld = false;
	bScaleMomentumByMass = false;
	DamageImpulse = 200000;
	bRadialDamageVelChange = false;
	DestructibleImpulse = 0;
	DestructibleDamageSpreadScale = 0;
	DamageFalloff = 1;
}


