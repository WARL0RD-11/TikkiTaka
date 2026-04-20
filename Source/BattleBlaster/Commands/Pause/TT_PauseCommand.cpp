// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PauseCommand.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

TT_PauseCommand::TT_PauseCommand(APlayerController* InPlayerController, TSubclassOf<UUserWidget> InPauseMenuClass)
	: CachedPlayerController(InPlayerController)
	, PauseMenuClass(InPauseMenuClass)
{
}

void TT_PauseCommand::Execute(
	ATT_BasePawn* InPawn,
	const FInputActionValue& InValue,
	const float& InSpeedModifier,
	const float& InTurnModifier)
{
	TogglePause();
}

void TT_PauseCommand::Execute(
	ATT_BasePawn* InPawn,
	const FInputActionValue& InValue,
	const FVector& InDirection,
	const float& InAimModifier)
{
	TogglePause();
}

void TT_PauseCommand::TogglePause()
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC)
	{
		return;
	}

	UWorld* World = PC->GetWorld();
	if (!World)
	{
		return;
	}

	const bool bIsPaused = UGameplayStatics::IsGamePaused(World);

	if (!bIsPaused)
	{
		UGameplayStatics::SetGamePaused(World, true);

		if (!PauseMenuInstance.IsValid() && PauseMenuClass)
		{
			UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, PauseMenuClass);
			PauseMenuInstance = NewWidget;
		}

		if (PauseMenuInstance.IsValid() && !PauseMenuInstance->IsInViewport())
		{
			PauseMenuInstance->AddToViewport();
		}

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());

		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}
	else
	{
		if (PauseMenuInstance.IsValid())
		{
			PauseMenuInstance->RemoveFromParent();
		}

		UGameplayStatics::SetGamePaused(World, false);

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}