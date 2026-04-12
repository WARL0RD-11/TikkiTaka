// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_MoveCommand.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "InputActionValue.h"

TT_MoveCommand::TT_MoveCommand()
{
}

void TT_MoveCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier, 
	const float& InTurnModifier)
{

	FVector MoveValue = InValue.Get<FVector>();

	if (!InPawn || MoveValue.IsZero()) return;

	FVector NormalizedMoveValue = MoveValue.GetSafeNormal();	

	const float DeltaSeconds = InPawn->GetWorld() ? InPawn->GetWorld()->GetDeltaSeconds() : 0.f;

	FVector LocalDelta(
		NormalizedMoveValue.Y * InSpeedModifier * DeltaSeconds,
		0.f,
		0.f
	);

	InPawn->AddActorLocalOffset(LocalDelta, true);

	InPawn->AddActorLocalRotation(FRotator(0.f, NormalizedMoveValue.X * InTurnModifier * DeltaSeconds, 0.f));
}

void TT_MoveCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection, 
	const float& InAimModifier)
{
}

