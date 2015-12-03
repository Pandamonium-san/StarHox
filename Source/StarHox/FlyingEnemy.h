// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "FlyingEnemy.generated.h"

UCLASS()
class STARHOX_API AFlyingEnemy : public APawn
{
	GENERATED_BODY()

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PlaneMesh;

public:
	// Sets default values for this pawn's properties
	AFlyingEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class ABasicProjectile> ProjectileClass;
	
	UPROPERTY(Category = Gameplay, BlueprintReadWrite, EditAnywhere)
		float Health;

	UPROPERTY(Category = Gameplay, BlueprintReadWrite, EditAnywhere)
		FVector Velocity;

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void Fire();
};
