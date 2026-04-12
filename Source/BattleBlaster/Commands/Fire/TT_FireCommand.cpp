// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_FireCommand.h"

TT_FireCommand::TT_FireCommand()
{
}

void TT_FireCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier, const float& InTurnModifier)
{
}

void TT_FireCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection, const float& InAimModifier)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Chitty Chitty Bang bang"));
}
