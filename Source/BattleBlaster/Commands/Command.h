// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ATT_BasePawn;
struct FInputActionValue;

class ICommand
{
public:
	virtual ~ICommand() {};
	virtual void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier, 
		const float& InTurnModifier) = 0;

	virtual void Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection, 
		const float& InAimModifier) = 0;
};