// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Prop.h"
#include "DamageType/StunDamage.h"
#include "DamageType/FireDamge.h"
#include "Robot.h"

AProp::AProp()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProp::BeginPlay()
{
	Super::BeginPlay();
	MaxHealth = Health;

	for (auto comp : GetComponentsByTag(UPrimitiveComponent::StaticClass(), "OnFire"))
	{
		if (auto c2 = Cast<UPrimitiveComponent>(comp))
		{
			c2->SetVisibility(false);
		}
	}
}

float AProp::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	FVector center = GetActorLocation();

	if (auto comp = FindComponentByClass<UStaticMeshComponent>()) center = comp->GetComponentLocation();

	if (ImmuneToNonFire && DamageEvent.DamageTypeClass != UFireDamge::StaticClass())
	{
		return 0;
	}

	if (DamageEvent.DamageTypeClass != UStunDamage::StaticClass())
	{
		Health -= FMath::Max(DamageAmount, 0.f);
	}

	if (Health / MaxHealth <= OnFirePct)
	{
		PrimaryActorTick.bCanEverTick = true;
	}

	if (Health / MaxHealth <= DetachPct)
	{
		for (auto& a : GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			if (auto prim = Cast<UPrimitiveComponent>(a))
			{
				if (!prim->IsSimulatingPhysics())
				{
					prim->SetSimulatePhysics(true);
					UGameplayStatics::PlaySoundAtLocation(this, DeathSound, center);
				}
			}
		}
	}

	if (Health <= 0)
	{
		Destroy();

		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, center);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion, center);

		TArray<FOverlapResult> res;

		if (GetWorld()->OverlapMultiByObjectType(res, center, FQuat::Identity, FCollisionObjectQueryParams::AllObjects, FCollisionShape::MakeSphere(ExplosionRadius)))
		{
			TMap<TWeakObjectPtr<AActor>, TArray<TWeakObjectPtr<UPrimitiveComponent>>> compsHit;

			for (auto& a : res)
			{
				if (a.Actor.IsValid() && a.Component.IsValid())
				{
					if (ExplosionDamage < 0 && !Cast<ARobot>(a.Actor.Get())) continue;

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
				damageEvent.Origin = center;
				damageEvent.Params.BaseDamage = ExplosionDamage;
				damageEvent.Params.DamageFalloff = 1;
				damageEvent.Params.InnerRadius = ExplosionRadius;
				damageEvent.Params.OuterRadius = ExplosionRadius * 2;
				damageEvent.Params.MinimumDamage = 5;

				if (a.Key.IsValid())
				{
					damageEvent.DamageTypeClass = ExplosionDamage > 0 ? UFireDamge::StaticClass() : UDamageType::StaticClass();
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

	bool onFire = Health / MaxHealth < OnFirePct;

	if (onFire)
	{
		TakeDamage(BurnRate * DeltaSeconds, FDamageEvent(), nullptr, nullptr);
	}

	for (auto comp : GetComponentsByTag(UPrimitiveComponent::StaticClass(), "OnFire"))
	{
		if (auto c2 = Cast<UPrimitiveComponent>(comp))
		{
			c2->SetVisibility(onFire);
		}
	}
}
