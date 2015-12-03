// Fill out your copyright notice in the Description page of Project Settings.

#include "StarHox.h"
#include "BasicObstacle.h"


// Sets default values
ABasicObstacle::ABasicObstacle()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Durability = 1.f;
}

// Called when the game starts or when spawned
void ABasicObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicObstacle::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABasicObstacle::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Obstacle hit!"));
	if (--Durability <= 0)
		Destroy();
}
