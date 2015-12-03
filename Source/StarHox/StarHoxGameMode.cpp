// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "StarHox.h"
#include "StarHoxGameMode.h"
#include "StarHoxPawn.h"
#include "Blueprint/UserWidget.h"

AStarHoxGameMode::AStarHoxGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AStarHoxPawn::StaticClass();
}

void AStarHoxGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget)
			CurrentWidget->AddToViewport();
	}
	GetWorldTimerManager().SetTimer(Timer, this, &AStarHoxGameMode::EndGame, 30.f, false);
}
	
void AStarHoxGameMode::EndGame()
{
	//UGameplayStatics::OpenLevel(this, TEXT("Menu"), false);
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	if (EndWidgetClass)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), EndWidgetClass);
		if (CurrentWidget)
			CurrentWidget->AddToViewport();
	}
}

void AStarHoxGameMode::AddTime(float time)
{
	float currentTime = GetWorldTimerManager().GetTimerRemaining(Timer);
	GetWorldTimerManager().SetTimer(Timer, this, &AStarHoxGameMode::EndGame, currentTime + time, false);
}
