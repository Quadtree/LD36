// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Robot.h"
#include "DamageType/PhysicalDamage.h"
#include "DamageType/StunDamage.h"
#include "Actor/Prop.h"
#include "Actor/Dialog.h"
#include "RobotMovementComponent.h"
#include "Actor/BaseWall.h"

// Sets default values
ARobot::ARobot(const FObjectInitializer& oi) : Super(oi.SetDefaultSubobjectClass<URobotMovementComponent>(ACharacter::CharacterMovementComponentName))
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
	Intact = true;

	GetWorld()->GetTimerManager().SetTimer(HalfSecondUpdateTimerHandle, this, &ARobot::HalfSecondUpdate, 0.5f, true, FMath::FRandRange(0,1));
	HalfSecondUpdate();
}

void ARobot::EndPlay(const EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

// Called every frame
void ARobot::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (OnFeet && ManualMovement.Size() > 0.1f && !TryKick)
	{
		float facing = FMath::Atan2(ManualMovement.Y, ManualMovement.X);

		FRotator dest = FRotator(0, FMath::RadiansToDegrees(facing) - 45, 0);
		FRotator newRot = FMath::RInterpConstantTo(GetActorRotation(), dest, DeltaTime, 1500);
		GetController()->SetControlRotation(newRot);

		AddMovementInput(GetActorRotation().RotateVector(FVector::ForwardVector), ManualMovement.Size());
	}
	else if(Target)
	{
		FRotator dest;

		if (Cast<AProp>(Target))
			dest = (Target->FindComponentByClass<UStaticMeshComponent>()->GetComponentLocation() - GetActorLocation()).ToOrientationRotator();
		else
			dest = (Target->GetActorLocation() - GetActorLocation()).ToOrientationRotator();

		dest.Pitch = 0;
		dest.Roll = 0;
		FRotator newRot = FMath::RInterpConstantTo(GetActorRotation(), dest, DeltaTime, 1500);
		GetController()->SetControlRotation(newRot);
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
	if (IsPunching) MeleeAttack(FistBoneName, PunchLockoutTimer, 30*(HasMace?2.5f:1.f), 20 * (HasMace ? 2.5f : 1.f), 55);

	KickLockoutTimer -= DeltaTime * ActionSpeed;
	PunchLockoutTimer -= DeltaTime * ActionSpeed;

	GetMovementComponent()->SetActive(OnFeet);

	if (TryKick) GetMovementComponent()->SetActive(false);

	if (Health <= 0) StunTime = 999999;

	MissileCharge += DeltaTime;

	if (MissileCharge >= 2 && TryFireMissile && OnFeet)
	{
		for (auto comp : GetComponentsByTag(UPrimitiveComponent::StaticClass(), "Missile"))
		{
			if (auto prim = Cast<UPrimitiveComponent>(comp))
			{
				if (GetWorld()->SpawnActor<AActor>(MissileType, prim->GetComponentLocation() + prim->GetComponentRotation().RotateVector(FVector(100,0,0)), prim->GetComponentRotation())) MissileCharge = 0;

				UGameplayStatics::PlaySoundAtLocation(this, MissileLaunchSound, GetActorLocation());

				if (MissileCharge < 2) break;
			}
			if (MissileCharge < 2) break;
		}
	}

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

	if ((IsKicking || IsPunching) && !Target) AcquireTarget();
	if (!(IsKicking || IsPunching) && Target) Target = nullptr;
}

void ARobot::HalfSecondUpdate()
{
	if (IsKicking || IsPunching) AcquireTarget();

	if (Cast<APlayerController>(GetController()))
	{
		TArray<FOverlapResult> res;
		if (GetWorld()->OverlapMultiByObjectType(res, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(100)))
		{
			for (auto a : res)
			{
				if (a.Actor.Get() == this) continue;

				if (auto rbt = Cast<ARobot>(a.Actor.Get()))
				{
					if (rbt->Health <= 0)
					{
						if (rbt->HasMissile && !HasMissile) { rbt->HasMissile = false; HasMissile = true; Say(TEXT("get_missile")); }
						if (rbt->HasMace && !HasMace) { rbt->HasMace = false; HasMace = true; Say(TEXT("get_mace")); }
					}
				}
			}
		}
	}

	if (!SDF_SawFirstOil && Cast<APlayerController>(GetController()))
	{
		TArray<FOverlapResult> res;
		if (GetWorld()->OverlapMultiByObjectType(res, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(300)))
		{
			for (auto a : res)
			{
				if (a.Actor.IsValid() && Cast<AProp>(a.Actor.Get()))
				{
					if (Cast<AProp>(a.Actor.Get())->ExplosionDamage < -1)
					{
						Say(TEXT("see_first_oil"));
						SDF_SawFirstOil = true;
					}
				}
			}
		}
	}
}

void ARobot::AcquireTarget()
{
	TArray<FOverlapResult> res;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	if (GetWorld()->OverlapMultiByObjectType(res, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(300), params))
	{
		float bestAngleFromFront = 100000;
		Target = nullptr;

		for (auto a : res)
		{
			auto prop = Cast<AProp>(a.Actor.Get());
			auto robot = Cast<ARobot>(a.Actor.Get());

			if (a.Actor.IsValid() && ((robot && robot->Health > 0) || prop) && !Cast<ABaseWall>(a.Actor.Get()))
			{
				FVector trgCenter;

				if (prop)
					trgCenter = prop->FindComponentByClass<UStaticMeshComponent>()->GetComponentLocation();
				else
					trgCenter = robot->GetActorLocation();

				FVector delta = (trgCenter - GetActorLocation());

				FRotator ang = delta.ToOrientationRotator();

				if (FMath::Abs((ang - GetActorRotation()).Yaw) < 100)
				{
					float offsetAng = (ang - GetActorRotation()).Yaw + (prop ? 10000 : 0) + delta.Size2D() / 3;

					//DrawDebugLine(GetWorld(), GetActorLocation(), a.Actor->GetActorLocation(), FColor::Purple, true, 0.5f);
					//DrawDebugString(GetWorld(), (GetActorLocation() + a.Actor->GetActorLocation())/2, *FString::FromInt(offsetAng), nullptr, FColor::Purple, true, 0.5f);

					if (FMath::Abs(offsetAng) < bestAngleFromFront)
					{
						FCollisionQueryParams p2;
						p2.AddIgnoredActor(this);
						p2.AddIgnoredActor(a.Actor.Get());

						if (!GetWorld()->LineTraceTestByChannel(GetActorLocation() + FVector(0, 0, 50), trgCenter + FVector(0, 0, 50), ECollisionChannel::ECC_Visibility, p2))
						{
							bestAngleFromFront = FMath::Abs(offsetAng);
							Target = a.Actor.Get();
						}
					}
				}
			}
		}

		if (Target)
		{
			//DrawDebugBox(GetWorld(), Target->GetActorLocation(), FVector(30, 30, 30), FColor::Purple, true, 0.5f);
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
	InputComponent->BindAxis("LaunchMissile", this, &ARobot::SetTryFireMissile);
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
		UE_LOG(LogTemp, Display, TEXT("%s took %s damage"), *GetName(), *FString::SanitizeFloat(DamageAmount));
		Health = FMath::Min(Health - DamageAmount, MaxHealth);
	}

	UpdateStandingStatus();

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ARobot::Say(FName key, float delay)
{
	if (Health <= 0) return;

	auto dialog = GetWorld()->SpawnActor<ADialog>();
	dialog->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

	dialog->Duration = 4;
	dialog->SetKey(key);
	dialog->StartDelay = delay;
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

	//DrawDebugSphere(GetWorld(), attackLocation, DAMAGE_AREA_RADIUS, 5, FColor::Red);

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

				UGameplayStatics::PlaySoundAtLocation(this, AttackHitSound, GetActorLocation());
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

	if (Health <= 0 && Intact)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion, GetActorLocation() + FVector(0,0,50));

		for (auto con : GetMesh()->Constraints)
		{
			con->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 10000);
			con->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 10000);
			con->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 10000);

			con->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 10000);
			con->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, 10000);
			con->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Free, 10000);
		}

		Intact = false;
	}
}

