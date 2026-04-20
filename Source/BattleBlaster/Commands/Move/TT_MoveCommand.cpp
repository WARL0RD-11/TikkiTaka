// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_MoveCommand.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "InputActionValue.h"

TT_MoveCommand::TT_MoveCommand()
{
}

void TT_MoveCommand::ExecuteForward(ATT_BasePawn* InPawn, float InputValue, float DeltaTime, float MoveSpeed)
{
	if (!InPawn || FMath::IsNearlyZero(InputValue))
	{
		return;
	}

	const float ClampedInput = FMath::Clamp(InputValue, -1.f, 1.f);

	const FVector LocalDelta(
		ClampedInput * MoveSpeed * DeltaTime,
		0.f,
		0.f
	);

	InPawn->AddActorLocalOffset(LocalDelta, true);
}

void TT_MoveCommand::ExecuteTurn(ATT_BasePawn* InPawn, float InputValue, float DeltaTime, float TurnSpeed)
{
	if (!InPawn || FMath::IsNearlyZero(InputValue))
	{
		return;
	}

	const float ClampedInput = FMath::Clamp(InputValue, -1.f, 1.f);

	InPawn->AddActorLocalRotation(
		FRotator(0.f, ClampedInput * TurnSpeed * DeltaTime, 0.f)
	);
}

void TT_MoveCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier, 
	const float& InTurnModifier)
{

}

void TT_MoveCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection, 
	const float& InAimModifier)
{
}

void TT_MoveCommand::ExecuteMoveToLocation(ATT_BasePawn* InPawn, const FVector& TargetLocation, float DeltaTime, float MoveSpeed, float TurnSpeed)
{
	if (!InPawn)
	{
		return;
	}

	const FVector ToTarget = TargetLocation - InPawn->GetActorLocation();
	const FVector ToTarget2D(ToTarget.X, ToTarget.Y, 0.f);

	if (ToTarget2D.IsNearlyZero())
	{
		return;
	}

	const FVector Forward = InPawn->GetActorForwardVector();
	const FVector DesiredDir = ToTarget2D.GetSafeNormal();

	const float CrossZ = FVector::CrossProduct(Forward, DesiredDir).Z;
	const float Dot = FVector::DotProduct(Forward, DesiredDir);

	const float TurnInput = FMath::Clamp(CrossZ, -1.f, 1.f);
	const float ForwardInput = Dot > 0.2f ? 1.f : 0.3f;

	InPawn->RotateBase(TurnInput * TurnSpeed);
	InPawn->MoveForward(ForwardInput * MoveSpeed * DeltaTime);
}

