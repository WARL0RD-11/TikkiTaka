// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_AimCommand.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "InputActionValue.h"


TT_AimCommand::TT_AimCommand()
{
}

void TT_AimCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier,
	const float& InTurnModifier)
{

}

void TT_AimCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection, const float& InAimModifier)
{
	FVector LookAtTarget = InDirection - InPawn->TurretMeshComponent->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, LookAtTarget.Rotation().Yaw, 0.f);	

	FRotator SmoothRotation = FMath::RInterpTo( InPawn->TurretMeshComponent->GetComponentRotation(), 
		LookAtRotation, InPawn->GetWorld()->GetDeltaSeconds(), InAimModifier);

	InPawn->TurretMeshComponent->SetWorldRotation(SmoothRotation);
}
