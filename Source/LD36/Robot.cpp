// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Robot.h"
#include "DamageType/PhysicalDamage.h"
#include "DamageType/StunDamage.h"
#include "Actor/Prop.h"

// Sets default values
ARobot::ARobot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ManualMovementMode = false;
	OnFeet = true;
	StunTime = 0;
	ActionSpeed = 1;
}

// Called when the game starts or when spawned
void ARobot::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultMeshRelativeTransform = GetMesh()->GetRelativeTransform();
	FallBlend = 0;
}

// Called every frame
void ARobot::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (OnFeet && ManualMovement.Size() > 0.1f && !TryKick)
	{
		float facing = FMath::Atan2(ManualMovement.Y, ManualMovement.X);
		//UE_LOG(LogTemp, Display, TEXT("facing=%s"), *FString::SanitizeFloat(facing));

		/*if (auto cam = FindComponentByClass<UCameraComponent>())
		{
			UE_LOG(LogTemp, Display, TEXT("facing=%s"), *FString::SanitizeFloat(cam->GetComponentRotation().Yaw));
			facing += cam->GetComponentRotation().Yaw;
		}*/

		FRotator dest = FRotator(0, FMath::RadiansToDegrees(facing) - 45, 0);
		FRotator newRot = FMath::RInterpConstantTo(GetActorRotation(), dest, DeltaTime, 1500);
		//UE_LOG(LogTemp, Display, TEXT("oldRot=%s newRot=%s"), *GetActorRotation().ToString(), *newRot.ToString());

		GetController()->SetControlRotation(newRot);

		AddMovementInput(GetActorRotation().RotateVector(FVector::ForwardVector), ManualMovement.Size());
	}

	UpdateStandingStatus();

	StunTime = FMath::Max(StunTime - DeltaTime, 0.f);

	IsKicking = false;
	IsPunching = false;

	if (OnFeet)
	{
		if (TryKick && KickLockoutTimer <= 0) IsKicking = true;
		if (TryPunch && PunchLockoutTimer <= 0) IsPunching = true;
	}

	if (IsKicking) MeleeAttack(FootBoneName, KickLockoutTimer, 20, 40, 25);
	if (IsPunching) MeleeAttack(FistBoneName, PunchLockoutTimer, 30, 20, 55);

	KickLockoutTimer -= DeltaTime * ActionSpeed;
	PunchLockoutTimer -= DeltaTime * ActionSpeed;

	GetMovementComponent()->SetActive(OnFeet);

	if (TryKick) GetMovementComponent()->SetActive(false);

	if (Health <= 0) StunTime = 999999;

	for (auto comp : GetComponentsByTag(UPrimitiveComponent::StaticClass(), "Missile"))
	{
		if (auto prim = Cast<UPrimitiveComponent>(comp))
		{
			prim->SetVisibility(HasMissile);
		}
	}

	for (auto comp : GetComponentsByTag(UPrimitiveComponent::StaticClass(), "Mace"))
	{
		if (auto prim = Cast<UPrimitiveComponent>(comp))
		{
			prim->SetVisibility(HasMace);
		}
	}

	MissileCharge += DeltaTime;

	if (MissileCharge >= 2 && TryFireMissile)
	{
		for (auto comp : GetComponentsByTag(UPrimitiveComponent::StaticClass(), "Missile"))
		{
			if (auto prim = Cast<UPrimitiveComponent>(comp))
			{
				if (GetWorld()->SpawnActor<AActor>(MissileType, prim->GetComponentLocation() + prim->GetComponentRotation().RotateVector(FVector(100,0,0)), prim->GetComponentRotation())) MissileCharge = 0;

				if (MissileCharge < 2) break;
			}
			if (MissileCharge < 2) break;
		}
	}
}

// Called to bind functionality to input
void ARobot::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MovementLeftRight", this, &ARobot::SetMovementLeftRight);
	InputComponent->BindAxis("MovementUpDown", this, &ARobot::SetMovementUpDown);

	InputComponent->BindAxis("Punch", this, &ARobot::SetTryPunch);
	InputComponent->BindAxis("Kick", this, &ARobot::SetTryKick);
}

float ARobot::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID) && (DamageEvent.DamageTypeClass == UStunDamage::StaticClass() || DamageEvent.DamageTypeClass == UPhysicalDamage::StaticClass()))
	{
		FRadialDamageEvent& rde = (FRadialDamageEvent&)DamageEvent;
		float distanceToCore = FMath::Sqrt(FVector::DistSquaredXY(rde.Origin, GetActorLocation()));

		UE_LOG(LogTemp, Display, TEXT("distanceToCore=%s"), *FString::SanitizeFloat(distanceToCore));

		if (distanceToCore > 25 && !IsPunching)
		{
			if (DamageEvent.DamageTypeClass == UStunDamage::StaticClass())
				DamageAmount /= 32;
			else
				DamageAmount /= 4;
		}
	}

	if (DamageEvent.DamageTypeClass == UStunDamage::StaticClass())
	{
		StunTime += FMath::Max(0.f, DamageAmount * FMath::FRandRange(1.f / 60.f, 10.f / 60.f) - (5.f / 60.f));
	}
	else
	{
		Health = FMath::Min(Health - DamageAmount, MaxHealth);
	}

	UpdateStandingStatus();

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ARobot::SetMovementLeftRight(float value)
{
	ManualMovement.X = -value;
	ManualMovementMode = true;
}

void ARobot::SetMovementUpDown(float value)
{
	ManualMovement.Y = value;
	ManualMovementMode = true;
}

void ARobot::SetTryPunch(float value)
{
	TryPunch = value > 0.5f;
}

void ARobot::SetTryKick(float value)
{
	TryKick = value > 0.5f;
}

void ARobot::SetTryFireMissile(float value)
{
	TryFireMissile = value >= 0.5f;
}

void ARobot::MeleeAttack(const FName& boneName, float& lockoutTimer, float damage, float stunDamage, float minCoreDistance)
{
	FVector attackLocation = GetMesh()->GetBoneLocation(boneName);

	float distanceFromCore = FMath::Sqrt(FVector::DistSquaredXY(attackLocation, GetActorLocation()));

	if (distanceFromCore < minCoreDistance) return;

	TArray<FOverlapResult> res;

	const float DAMAGE_AREA_RADIUS = 15;

	DrawDebugSphere(GetWorld(), attackLocation, DAMAGE_AREA_RADIUS, 5, FColor::Red);

	if (GetWorld()->OverlapMultiByObjectType(res, attackLocation, FQuat::Identity, FCollisionObjectQueryParams::AllObjects, FCollisionShape::MakeSphere(DAMAGE_AREA_RADIUS)))
	{
		TMap<TWeakObjectPtr<AActor>, TArray<TWeakObjectPtr<UPrimitiveComponent>>> compsHit;

		for (auto& a : res)
		{
			if (a.Actor.Get() == this) continue;

			if (!Cast<ARobot>(a.Actor.Get()) && !Cast<AProp>(a.Actor.Get())) continue;

			if (a.Actor.Get() && a.Component.Get())
			{
				if (!Cast<USkinnedMeshComponent>(a.Component.Get()) && !Cast<UStaticMeshComponent>(a.Component.Get())) continue;

				if (!compsHit.Contains(a.Actor)) compsHit.Add(a.Actor, TArray<TWeakObjectPtr<UPrimitiveComponent>>());
				
				compsHit[a.Actor].Add(a.Component);
			}
		}

		
		for (auto& a : compsHit)
		{
			UE_LOG(LogTemp, Display, TEXT("distanceFromCore=%s"), *FString::SanitizeFloat(distanceFromCore));

			if (distanceFromCore >= minCoreDistance)
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
				damageEvent.Origin = attackLocation;
				damageEvent.Params.BaseDamage = 20;
				damageEvent.Params.DamageFalloff = 1;
				damageEvent.Params.InnerRadius = 50;
				damageEvent.Params.OuterRadius = 100;
				damageEvent.Params.MinimumDamage = 5;

				if (a.Key.IsValid())
				{
					damageEvent.DamageTypeClass = UPhysicalDamage::StaticClass();
					a.Key->TakeDamage(damage, damageEvent, nullptr, nullptr);
				}

				if (a.Key.IsValid())
				{
					damageEvent.DamageTypeClass = UStunDamage::StaticClass();
					a.Key->TakeDamage(stunDamage, damageEvent, nullptr, nullptr);
				}
			}

			lockoutTimer = 0.5f;
		}
	}
}

void ARobot::UpdateStandingStatus()
{
	if (StunTime > 0 && OnFeet)
	{
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMovementComponent()->SetActive(false);
		GetMesh()->SetPhysicsBlendWeight(1);
		GetMesh()->SetEnablePhysicsBlending(true);
		FallBlend = 1;
		OnFeet = false;
	}

	if (!OnFeet)
	{
		if (StunTime > 0)
		{
			//FallBlend -= DeltaTime * 0.6f;
			GetMesh()->SetPhysicsBlendWeight(FMath::Clamp((StunTime*0.6f) + 0.75f, 0.f, 1.f));
		}
		else
		{
			GetMesh()->SetSimulatePhysics(false);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMovementComponent()->SetActive(true);
			GetMesh()->SetEnablePhysicsBlending(false);
			OnFeet = true;
		}
	}

	if (!OnFeet && FallBlend == 1)
	{
		OnGroundLoc = GetMesh()->GetBoneLocation("Bone");
	}

	if (!OnFeet)
	{
		//DrawDebugSphere(GetWorld(), OnGroundLoc, 50, 5, FColor::Red);
		//DrawDebugSphere(GetWorld(), GetCapsuleComponent()->GetComponentLocation(), 50, 5, FColor::Green);

		GetCapsuleComponent()->SetWorldLocation(FVector(OnGroundLoc.X, OnGroundLoc.Y, GetCapsuleComponent()->GetComponentLocation().Z));
	}
}

