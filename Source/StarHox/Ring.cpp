// Fill out your copyright notice in the Description page of Project Settings.

#include "StarHox.h"
#include "StarHoxPawn.h"
#include "StarHoxGameMode.h"
#include "Ring.h"

// Sets default values
ARing::ARing()
{
	RingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = RingMesh;
}

// Called when the game starts or when spawned
void ARing::BeginPlay()
{
	Super::BeginPlay();
	RingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnBeginOverlap);
}

// Called every frame
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARing::OnBeginOverlap(AActor *OtherActor, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ring overlap!"));
	AStarHoxPawn* pawn;
	pawn = Cast<AStarHoxPawn>(OtherActor);

	AStarHoxGameMode* gameMode = (AStarHoxGameMode*)GetWorld()->GetAuthGameMode();
	if (pawn && gameMode)
	{
		pawn->CurrentBoost += 20.f;
		gameMode->AddTime(3.f);
		Destroy();
	}
}



