// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_FireCommand.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "Components/ArrowComponent.h"
#include "InputActionValue.h"

TT_FireCommand::TT_FireCommand()
{
}

void TT_FireCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const float& InSpeedModifier, const float& InTurnModifier)
{
}

void TT_FireCommand::Execute(ATT_BasePawn* InPawn, const FInputActionValue& InValue, const FVector& InDirection, const float& InAimModifier)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Chitty Chitty Bang bang"));

	FVector SpwanLoc = InPawn->ProjectileSpawnPoint->GetComponentLocation();
	FRotator SpwanRot = InPawn->ProjectileSpawnPoint->GetComponentRotation();

	DrawDebugSphere(InPawn->GetWorld(), SpwanLoc, 25.f, 12, FColor::Red, false, 5.f);	
}
