// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_EnemyTank.h"
#include "BattleBlaster/AI/Tank/TT_TankAIComponent.h"

ATT_EnemyTank::ATT_EnemyTank()
{
	PrimaryActorTick.bCanEverTick = true;

	TankAIComponent = CreateDefaultSubobject<UTT_TankAIComponent>(TEXT("TankAIComponent"));
}

void ATT_EnemyTank::BeginPlay()
{
	Super::BeginPlay();
}

void ATT_EnemyTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATT_EnemyTank::HandleDestruction()
{
	Super::HandleDestruction();
}
