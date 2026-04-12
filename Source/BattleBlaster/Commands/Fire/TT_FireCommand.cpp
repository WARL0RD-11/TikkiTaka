// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_FireCommand.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "BattleBlaster/Projectile/TT_Missile.h"
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
	FVector SpwanLoc = InPawn->ProjectileSpawnPoint->GetComponentLocation();
	FRotator SpwanRot = InPawn->ProjectileSpawnPoint->GetComponentRotation();

	ATT_Missile* Missile = InPawn->GetWorld()->SpawnActor<ATT_Missile>(InPawn->MissileClass, SpwanLoc, SpwanRot);
	if (Missile)
	{
		Missile->SetOwner(InPawn);
	}
}
