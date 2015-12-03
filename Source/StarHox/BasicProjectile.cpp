// Fill out your copyright notice in the Description page of Project Settings.

#include "StarHox.h"
#include "BasicProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABasicProjectile::ABasicProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ABasicProjectile::OnHit);

	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 6000.0f;
	ProjectileMovement->MaxSpeed = 6000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ABasicProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("hit registered!"));
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{

	}
	Destroy();
}