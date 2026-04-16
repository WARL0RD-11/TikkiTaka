// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_EnemyTank.h"
#include "BattleBlaster/AI/Tank/TT_TankAIComponent.h"
#include "BattleBlaster/GameMode/TikkiTakaGameMode.h"	

ATT_EnemyTank::ATT_EnemyTank()
{
	PrimaryActorTick.bCanEverTick = true;

	TankAIComponent = CreateDefaultSubobject<UTT_TankAIComponent>(TEXT("TankAIComponent"));
}

void ATT_EnemyTank::BeginPlay()
{
	Super::BeginPlay();

	GM = Cast<ATikkiTakaGameMode>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->RegisterEnemyTank(this);
		bRegisteredWithGameMode = true;
	}
}

void ATT_EnemyTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATT_EnemyTank::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bRegisteredWithGameMode)
	{
		if (GM)
		{
			GM->UnregisterEnemyTank(this);
		}

		bRegisteredWithGameMode = false;
	}

	Super::EndPlay(EndPlayReason);
}

void ATT_EnemyTank::HandleDestruction()
{
	Super::HandleDestruction();
}
