// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Robot.h"
#include "DamageType/PhysicalDamage.h"
#include "DamageType/StunDamage.h"


// Sets default values
ARobot::ARobot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ManualMovementMode = false;
	OnFeet = true;
	StunTime = 0;
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

	if (OnFeet && ManualMovement.Size() > 0.1f)
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

		SetActorRotation(newRot);

		AddMovementInput(GetActorRotation().RotateVector(FVector::ForwardVector), ManualMovement.Size());
	}

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

	StunTime -= DeltaTime;

	IsKicking = false;
	IsPunching = false;

	if (OnFeet)
	{
		if (TryKick && KickLockoutTimer <= 0) IsKicking = true;
		if (TryPunch && PunchLockoutTimer <= 0) IsPunching = true;
	}

	if (IsKicking) MeleeAttack(FootBoneName, KickLockoutTimer, 20, 40);
	if (IsPunching) MeleeAttack(FistBoneName, PunchLockoutTimer, 30, 20);

	KickLockoutTimer -= DeltaTime;
	PunchLockoutTimer -= DeltaTime;
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

void ARobot::MeleeAttack(const FName& boneName, float& lockoutTimer, float damage, float stunDamage)
{
	FVector attackLocation = GetMesh()->GetBoneLocation(boneName);

	TArray<FOverlapResult> res;

	DrawDebugSphere(GetWorld(), attackLocation, 30, 5, FColor::Red);

	if (GetWorld()->OverlapMultiByObjectType(res, attackLocation, FQuat::Identity, FCollisionObjectQueryParams::AllObjects, FCollisionShape::MakeSphere(30)))
	{
		TMap<TWeakObjectPtr<AActor>, TArray<TWeakObjectPtr<UPrimitiveComponent>>> compsHit;

		for (auto& a : res)
		{
			if (a.Actor.Get() == this) continue;

			if (a.Actor.Get() && a.Component.Get())
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
			}
			damageEvent.Origin = attackLocation;
			
			damageEvent.DamageTypeClass = UPhysicalDamage::StaticClass();
			a.Key->TakeDamage(damage, damageEvent, nullptr, nullptr);

			damageEvent.DamageTypeClass = UStunDamage::StaticClass();
			a.Key->TakeDamage(stunDamage, damageEvent, nullptr, nullptr);

			lockoutTimer = 0.5f;
		}
	}
}

