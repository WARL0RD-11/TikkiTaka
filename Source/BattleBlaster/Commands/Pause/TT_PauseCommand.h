// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleBlaster/Commands/Command.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

/**
 * 
 */
class APlayerController;	

class BATTLEBLASTER_API TT_PauseCommand : public ICommand
{
	public:
	TT_PauseCommand(APlayerController* InPlayerController, TSubclassOf<UUserWidget> InPauseMenuClass);
	virtual void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier, 
		const float& InTurnModifier) override;

	virtual void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection, 
		const float& InAimModifier) override;	

private:
	void TogglePause();

private:
	TWeakObjectPtr<APlayerController> CachedPlayerController;
	TSubclassOf<UUserWidget> PauseMenuClass;
	TWeakObjectPtr<UUserWidget> PauseMenuInstance;
};
