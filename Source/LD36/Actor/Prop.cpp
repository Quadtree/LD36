// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Prop.h"
#include "DamageType/StunDamage.h"
#include "DamageType/FireDamge.h"

AProp::AProp()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProp::BeginPlay()
{
	Super::BeginPlay();
	MaxHealth = Health;
}

float AProp::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (DamageEvent.DamageTypeClass != UStunDamage::StaticClass())
	{
		Health -= FMath::Max(DamageAmount, 0.f);
	}

	if (Health / MaxHealth <= DetachPct)
	{
		for (auto& a : GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			if (auto prim = Cast<UPrimitiveComponent>(a))
			{
				prim->SetSimulatePhysics(true);
			}
		}
	}

	if (Health <= 0)
	{
		Destroy();

		TArray<FOverlapResult> res;

		if (GetWorld()->OverlapMultiByObjectType(res, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllObjects, FCollisionShape::MakeSphere(ExplosionRadius)))
		{
			TMap<TWeakObjectPtr<AActor>, TArray<TWeakObjectPtr<UPrimitiveComponent>>> compsHit;

			for (auto& a : res)
			{
				if (a.Actor.IsValid() && a.Component.IsValid())
				{
					if (!compsHit.Contains(a.Actor)) compsHit.Add(a.Actor, TArray<TWeakObjectPtr<UPrimitiveComponent>>());
					compsHit[a.Actor].Add(a.Component);
				}
			}


			for (auto& a : compsHit)
			{
				FRadialDamageEvent damageEvent;

				for (auto& c : a.Value)
				{
					FHitResult hitRes;
					hitRes.Actor = a.Key;
					hitRes.bBlockingHit = true;
					hitRes.Component = c;

					damageEvent.ComponentHits.Add(hitRes);
				}
				damageEvent.Origin = GetActorLocation();
				damageEvent.Params.BaseDamage = ExplosionDamage;
				damageEvent.Params.DamageFalloff = 1;
				damageEvent.Params.InnerRadius = ExplosionRadius;
				damageEvent.Params.OuterRadius = ExplosionRadius * 2;
				damageEvent.Params.MinimumDamage = 5;

				if (a.Key.IsValid())
				{
					damageEvent.DamageTypeClass = UFireDamge::StaticClass();
					a.Key->TakeDamage(ExplosionDamage, damageEvent, nullptr, nullptr);
				}

				if (a.Key.IsValid())
				{
					damageEvent.DamageTypeClass = UStunDamage::StaticClass();
					a.Key->TakeDamage(ExplosionDamage, damageEvent, nullptr, nullptr);
				}
			}
		}
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AProp::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Health / MaxHealth < OnFirePct)
	{
		TakeDamage(BurnRate * DeltaSeconds, FDamageEvent(), nullptr, nullptr);
	}
}
