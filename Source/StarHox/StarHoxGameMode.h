// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "StarHoxGameMode.generated.h"

UCLASS(minimalapi)
class AStarHoxGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AStarHoxGameMode();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> EndWidgetClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Gameplay)
	FTimerHandle Timer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bVictory;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = StarHox)
	void EndGame();

	UFUNCTION(BlueprintCallable, Category = StarHox)
	void AddTime(float time);
};



