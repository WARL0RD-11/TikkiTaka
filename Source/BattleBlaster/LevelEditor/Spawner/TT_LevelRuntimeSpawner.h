
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Data/TT_CustomLevelTypes.h"
#include "TT_LevelRuntimeSpawner.generated.h"

class ATT_EnemyTank;
class ATT_TowerPawn;
class ATT_TankPawn;

UCLASS()
class BATTLEBLASTER_API ATT_LevelRuntimeSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATT_LevelRuntimeSpawner();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	TSubclassOf<ATT_EnemyTank> EnemyTankClass;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	TSubclassOf<ATT_TowerPawn> EnemyTowerClass;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	TSubclassOf<AActor> WallClass;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	TSubclassOf<AActor> PatrolPointClass;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	TSubclassOf<ATT_TankPawn> PlayerTankClass;

protected:
	void BuildLevel();

};
