// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "PhysicalDamage.h"


UPhysicalDamage::UPhysicalDamage()
{
	bCausedByWorld = false;
	bScaleMomentumByMass = false;
	DamageImpulse = 0;
	bRadialDamageVelChange = false;
	DestructibleImpulse = 0;
	DestructibleDamageSpreadScale = 0;
	DamageFalloff = 1;
}


