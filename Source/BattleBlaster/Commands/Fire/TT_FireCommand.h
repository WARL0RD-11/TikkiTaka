// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleBlaster/Commands/Command.h"
#include "CoreMinimal.h"

/**
 * 
 */
class BATTLEBLASTER_API TT_FireCommand : public ICommand
{
public:
	TT_FireCommand();
	void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier,
		const float& InTurnModifier) override;

	void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection,
		const float& InAimModifier) override;


};
