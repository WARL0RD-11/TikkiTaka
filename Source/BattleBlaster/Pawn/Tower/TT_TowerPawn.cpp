// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_TowerPawn.h"
#include "BattleBlaster/GameMode/TikkiTakaGameMode.h"
#include "BattleBlaster/AI/Tower/TT_TowerAIComponent.h"	
#include "Kismet/GameplayStatics.h"

void ATT_TowerPawn::BeginPlay()
{
	Super::BeginPlay();

	GM = Cast<ATikkiTakaGameMode>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->RegisterTower(this);
		bRegisteredWithGameMode = true;
	}
}

void ATT_TowerPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bRegisteredWithGameMode)
	{
		if (GM)
		{
			GM->UnregisterTower(this);
		}

		bRegisteredWithGameMode = false;
	}

	Super::EndPlay(EndPlayReason);
}

ATT_TowerPawn::ATT_TowerPawn()
{
	TowerAIComponent = CreateDefaultSubobject<UTT_TowerAIComponent>(TEXT("TowerAIComponent"));	
}

void ATT_TowerPawn::HandleDestruction()
{
	Super::HandleDestruction();

	// tower-specific VFX/SFX/disable collision 
}