// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Missile.h"
#include "DamageType/FireDamge.h"
#include "DamageType/StunDamage.h"


// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	TArray<FOverlapResult> res;
	FCollisionQueryParams p;
	p.AddIgnoredActor(this);
	p.AddIgnoredActor(this->GetInstigator());

	if (GetWorld()->OverlapAnyTestByObjectType(GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllObjects, FCollisionShape::MakeSphere(25), p))
	{
		Destroy();

		if (GetWorld()->OverlapMultiByObjectType(res, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllObjects, FCollisionShape::MakeSphere(ExplosionRadius), p))
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
				damageEvent.Params.OuterRadius = ExplosionRadius*2;
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
}

