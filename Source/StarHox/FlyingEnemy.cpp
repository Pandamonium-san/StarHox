// Fill out your copyright notice in the Description page of Project Settings.

#include "StarHox.h"
#include "FlyingEnemy.h"
#include "BasicProjectile.h"

// Sets default values
AFlyingEnemy::AFlyingEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	RootComponent = PlaneMesh;
	Health = 3;
}

// Called when the game starts or when spawned
void AFlyingEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlyingEnemy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	AddActorWorldOffset(Velocity * DeltaTime, true);
}

// Called to bind functionality to input
void AFlyingEnemy::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AFlyingEnemy::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Enemy hit!"));
	if (--Health <= 0)
		Destroy();
}

void AFlyingEnemy::Fire()
{
	if (ProjectileClass != NULL)
	{
		FVector CameraLoc = GetActorLocation();
		FRotator CameraRot = GetActorRotation();
		FVector const MuzzleLocation = CameraLoc + FTransform(CameraRot).TransformVector(MuzzleOffset);

		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			ABasicProjectile* Projectile = World->SpawnActor<ABasicProjectile>(ProjectileClass, MuzzleLocation, CameraRot, SpawnParams);
		}
	}
}
