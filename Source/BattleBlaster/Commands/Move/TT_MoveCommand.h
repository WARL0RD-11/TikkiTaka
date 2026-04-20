// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleBlaster/Commands/Command.h"
#include "CoreMinimal.h"

/**
 * 
 */
class BATTLEBLASTER_API TT_MoveCommand : public ICommand
{
public:
	TT_MoveCommand();

	void ExecuteForward(ATT_BasePawn* InPawn, float InputValue, float DeltaTime, float MoveSpeed);
	void ExecuteTurn(ATT_BasePawn* InPawn, float InputValue, float DeltaTime, float TurnSpeed);

	void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier, 
		const float& InTurnModifier) override;
	
	void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection,
		const float& InAimModifier) override;

	void ExecuteMoveToLocation(ATT_BasePawn* InPawn, const FVector& TargetLocation, float DeltaTime, 
		float MoveSpeed, float TurnSpeed);
};
