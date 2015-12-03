// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "StarHox.h"
#include "StarHoxPawn.h"
#include "BasicProjectile.h"

AStarHoxPawn::AStarHoxPawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 150.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.f;
	SpringArm->CameraLagMaxDistance = 150.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.0f;
	SpringArm->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set gameplay parameters
	MaxHealth = 50.f;
	CurrentHealth = MaxHealth;
	MaxBoost = 100.f;
	CurrentBoost = MaxBoost;

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 100.f;
	MoveSpeed = 1200.f;
	MaxSpeed = 2500.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 1000.f;

	MuzzleOffset = FVector(0.f, 0.f, 0.f);
}

void AStarHoxPawn::Tick(float DeltaSeconds)
{
	// Lock camera yaw and roll
	Camera->SetWorldRotation(FRotator(Camera->GetComponentRotation().Pitch, 0.f, 0.f));

	// Calculate change in position this frame
	DeltaOffset.X += CurrentForwardSpeed * DeltaSeconds;
	DeltaOffset.Y += CurrentHorizontalSpeed * DeltaSeconds;
	DeltaOffset.Z += CurrentVerticalSpeed * DeltaSeconds;

	// Move plane
	AddActorWorldOffset(DeltaOffset, true);
	DeltaOffset = FVector(0.f, 0.f, 0.f);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// Clamp rotations
	float ClampedPitch = FMath::Clamp<float>(GetActorRotation().Pitch, -25.f, 25.f);
	float ClampedRoll = FMath::Clamp<float>(GetActorRotation().Roll, -45.f, 45.f);
	SetActorRotation(FRotator(ClampedPitch, 0.f, ClampedRoll));

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AStarHoxPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit detected!"));
	// Set forward speed to zero upon collision
	CurrentForwardSpeed = -300.f;
	DeltaOffset += HitNormal * 100.f;

	// Set a timer for invulnerability reduce health if not already invulnerable
	if (!bInvulnerable)
	{
		CurrentHealth -= 10;
		ToggleInvulnerability();
		GetWorldTimerManager().SetTimer(InvulnerabilityTimer, this, &AStarHoxPawn::ToggleInvulnerability, 1.f, false);
	}
}

void AStarHoxPawn::ToggleInvulnerability()
{
	bInvulnerable = !bInvulnerable;
}

void AStarHoxPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Bind our control axis' to callback functions
	InputComponent->BindAxis("Thrust", this, &AStarHoxPawn::ThrustInput);
	InputComponent->BindAxis("MoveUp", this, &AStarHoxPawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &AStarHoxPawn::MoveRightInput);
	InputComponent->BindAction("Fire", IE_Pressed, this, &AStarHoxPawn::OnFire);
}

void AStarHoxPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	float CurrentAcc;
	if (bHasInput && CurrentBoost > 0)
	{
		CurrentAcc = Val * Acceleration;
		CurrentBoost -= 20 * GetWorld()->GetDeltaSeconds();
		SpringArm->CameraLagMaxDistance = 400.f;
	}
	else
	{
		if (CurrentForwardSpeed > MinSpeed)
			CurrentAcc = -0.5f * Acceleration;
		else
			CurrentAcc = Acceleration;
		CurrentBoost = FMath::Clamp(CurrentBoost, 0.f, MaxBoost);
		SpringArm->CameraLagMaxDistance = FMath::FInterpTo(SpringArm->CameraLagMaxDistance, 200.f, GetWorld()->GetDeltaSeconds(), 2.f);
	}
	CurrentBoost += 5 * GetWorld()->GetDeltaSeconds();
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, -1000.f, MaxSpeed);
}

void AStarHoxPawn::MoveUpInput(float Val)
{
	Val = FMath::Clamp(Val, -1.f, 1.f);

	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	float TargetPitchSpeed = bIsTurning ? (Val * TurnSpeed * -1.f) : (GetActorRotation().Pitch * -2.f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 5.f);

	float TargetVerticalSpeed = MoveSpeed * Val * -1.f;
	CurrentVerticalSpeed = FMath::FInterpTo(CurrentVerticalSpeed, TargetVerticalSpeed, GetWorld()->GetDeltaSeconds(), 5.f);
}

void AStarHoxPawn::MoveRightInput(float Val)
{
	Val = FMath::Clamp(Val, -1.f, 1.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (Val * TurnSpeed) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 5.f);

	float TargetHorizontalSpeed = MoveSpeed * Val;
	CurrentHorizontalSpeed = FMath::FInterpTo(CurrentHorizontalSpeed, TargetHorizontalSpeed, GetWorld()->GetDeltaSeconds(), 5.f);
}

void AStarHoxPawn::OnFire()
{
	if (ProjectileClass != NULL)
	{
		FVector CameraLoc = GetActorLocation();
		FRotator CameraRot = GetActorRotation();
		FVector const MuzzleLocation = CameraLoc + FTransform(CameraRot).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = CameraRot;

		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			ABasicProjectile* Projectile = World->SpawnActor<ABasicProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
		}
	}
}