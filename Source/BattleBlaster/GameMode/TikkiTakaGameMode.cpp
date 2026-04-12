// Fill out your copyright notice in the Description page of Project Settings.


#include "TikkiTakaGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BattleBlaster/Pawn/Tank/TT_TankPawn.h"
#include "BattleBlaster/Pawn/Tower/TT_TowerPawn.h"

void ATikkiTakaGameMode::BeginPlay()
{
	Super::BeginPlay();
	PlayerTank = Cast<ATT_TankPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATikkiTakaGameMode::ActorDied(AActor* DeadActor)
{
	if (!IsValid(DeadActor))
	{
		return;
	}

	if (DeadActor == PlayerTank)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Died"));
		}

		PlayerTank->HandleDestruction();

		// TODO: Handle player lose condition here
		DeadActor->Destroy();
		return;
	}

	if (ATT_TowerPawn* Tower = Cast<ATT_TowerPawn>(DeadActor))
	{
		Tower->HandleDestruction();

		// Destroy triggers EndPlay, which removes tower from ActiveTowers
		DeadActor->Destroy();

		const int32 RemainingTowers = GetActiveTowerCount();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Green,
				FString::Printf(TEXT("Tower Destroyed. Remaining Towers: %d"), RemainingTowers)
			);
		}

		if (RemainingTowers == 0)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("All Towers Destroyed! You Win!"));
			}

			// TODO: Handle win condition here
		}

		return;
	}

	DeadActor->Destroy();
}

void ATikkiTakaGameMode::RegisterTower(ATT_TowerPawn* Tower)
{
	if (!IsValid(Tower)) return;

	ActiveTowers.Add(Tower);
}

void ATikkiTakaGameMode::UnregisterTower(ATT_TowerPawn* Tower)
{
	if (!IsValid(Tower)) return;

	ActiveTowers.Remove(Tower);
}

int32 ATikkiTakaGameMode::GetActiveTowerCount() const
{
	return ActiveTowers.Num();
}

