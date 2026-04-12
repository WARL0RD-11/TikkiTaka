// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_TowerPawn.h"
#include "BattleBlaster/GameMode/TikkiTakaGameMode.h"
#include "Kismet/GameplayStatics.h"

void ATT_TowerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (ATikkiTakaGameMode* GM = Cast<ATikkiTakaGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->RegisterTower(this);
		bRegisteredWithGameMode = true;
	}
}

void ATT_TowerPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bRegisteredWithGameMode)
	{
		if (ATikkiTakaGameMode* GM = Cast<ATikkiTakaGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->UnregisterTower(this);
		}

		bRegisteredWithGameMode = false;
	}

	Super::EndPlay(EndPlayReason);
}

void ATT_TowerPawn::HandleDestruction()
{
	Super::HandleDestruction();

	// tower-specific VFX/SFX/disable collision 
}