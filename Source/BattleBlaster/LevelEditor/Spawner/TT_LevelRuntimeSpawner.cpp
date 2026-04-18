

#include "TT_LevelRuntimeSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "../Save/TT_CustomLevelSaveGame.h"
#include "BattleBlaster/Pawn/Tank/TT_EnemyTank.h"
#include "BattleBlaster/Pawn/Tower/TT_TowerPawn.h"
#include "BattleBlaster/Pawn/Tank/TT_TankPawn.h"
#include "BattleBlaster/AI/Tank/TT_TankAIComponent.h"
#include "BattleBlaster/AI/Tower/TT_TowerAIComponent.h"

static const FString GCustomLevelSlotName = TEXT("CustomLevelSlot");
static const int32 GCustomLevelSlotIndex = 0;

ATT_LevelRuntimeSpawner::ATT_LevelRuntimeSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATT_LevelRuntimeSpawner::BeginPlay()
{
	Super::BeginPlay();
	BuildLevel();
	
}

void ATT_LevelRuntimeSpawner::BuildLevel()
{
	if (!UGameplayStatics::DoesSaveGameExist(GCustomLevelSlotName, GCustomLevelSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("No custom level save found."));
		return;
	}

	UTT_CustomLevelSaveGame* SaveObj = Cast<UTT_CustomLevelSaveGame>(
		UGameplayStatics::LoadGameFromSlot(GCustomLevelSlotName, GCustomLevelSlotIndex));

	if (!SaveObj) return;

	const TArray<FTT_CustomPlacedActor>& Records = SaveObj->SavedLevel.PlacedActors;

	TMap<FName, AActor*> SpawnedMap;
	TArray<TPair<ATT_EnemyTank*, FTT_CustomPlacedActor>> SpawnedTanks;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Pass 1: walls, patrol points, towers, tanks, player
	for (const FTT_CustomPlacedActor& Record : Records)
	{
		AActor* Spawned = nullptr;

		switch (Record.Type)
		{
		case ETT_CustomPlaceableType::Wall:
			if (WallClass)
			{
				Spawned = GetWorld()->SpawnActor<AActor>(WallClass, Record.Transform, Params);
			}
			break;

		case ETT_CustomPlaceableType::PatrolPoint:
			if (PatrolPointClass)
			{
				Spawned = GetWorld()->SpawnActor<AActor>(PatrolPointClass, Record.Transform, Params);
			}
			break;

		case ETT_CustomPlaceableType::EnemyTower:
			if (EnemyTowerClass)
			{
				ATT_TowerPawn* Tower = GetWorld()->SpawnActor<ATT_TowerPawn>(EnemyTowerClass, Record.Transform, Params);
				Spawned = Tower;

				if (Tower)
				{
					if (UTT_TowerAIComponent* AI = Tower->FindComponentByClass<UTT_TowerAIComponent>())
					{
						AI->ApplyCustomTuning(Record.TowerTuning);
					}
				}
			}
			break;

		case ETT_CustomPlaceableType::EnemyTank:
			if (EnemyTankClass)
			{
				ATT_EnemyTank* Tank = GetWorld()->SpawnActor<ATT_EnemyTank>(EnemyTankClass, Record.Transform, Params);
				Spawned = Tank;

				if (Tank)
				{
					if (UTT_TankAIComponent* AI = Tank->FindComponentByClass<UTT_TankAIComponent>())
					{
						AI->ApplyCustomTuning(Record.TankTuning);
					}

					SpawnedTanks.Add(TPair<ATT_EnemyTank*, FTT_CustomPlacedActor>(Tank, Record));
				}
			}
			break;

		case ETT_CustomPlaceableType::PlayerStart:
			if (PlayerTankClass)
			{
				Spawned = GetWorld()->SpawnActor<AActor>(PlayerTankClass, Record.Transform, Params);
			}
			break;

		default:
			break;
		}

		if (Spawned)
		{
			SpawnedMap.Add(Record.InstanceId, Spawned);
		}
	}

	// Pass 2: link patrol points to tanks
	for (const TPair<ATT_EnemyTank*, FTT_CustomPlacedActor>& Pair : SpawnedTanks)
	{
		ATT_EnemyTank* Tank = Pair.Key;
		const FTT_CustomPlacedActor& SourceRecord = Pair.Value;

		if (!Tank) continue;

		if (UTT_TankAIComponent* AI = Tank->FindComponentByClass<UTT_TankAIComponent>())
		{
			TArray<AActor*> PatrolActors;

			for (const FName& PatrolId : SourceRecord.LinkedPatrolPointIds)
			{
				if (AActor** Found = SpawnedMap.Find(PatrolId))
				{
					if (IsValid(*Found))
					{
						PatrolActors.Add(*Found);
					}
				}
			}

			AI->SetPatrolPoints(PatrolActors);
		}
	}
}

