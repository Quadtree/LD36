// Fill out your copyright notice in the Description page of Project Settings.

#include "LD36.h"
#include "Robot.h"


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

	if (OnFeet != StunTime <= 0)
	{
		if (StunTime > 0)
		{
			GetMesh()->SetSimulatePhysics(true);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetMovementComponent()->SetActive(false);
		}
		else
		{
			GetMesh()->SetSimulatePhysics(false);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMovementComponent()->SetActive(true);
			GetMesh()->AttachTo(GetCapsuleComponent());
			GetMesh()->SetRelativeTransform(DefaultMeshRelativeTransform);
		}

		OnFeet = StunTime <= 0;
	}

	StunTime -= DeltaTime;
}

// Called to bind functionality to input
void ARobot::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MovementLeftRight", this, &ARobot::SetMovementLeftRight);
	InputComponent->BindAxis("MovementUpDown", this, &ARobot::SetMovementUpDown);
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

