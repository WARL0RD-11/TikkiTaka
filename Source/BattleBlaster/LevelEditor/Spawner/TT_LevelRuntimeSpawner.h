#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Data/TT_CustomLevelTypes.h"
#include "TT_LevelRuntimeSpawner.generated.h"

class ATT_EnemyTank;
class ATT_TowerPawn;
class ATT_TankPawn;
class APlayerStart;

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
	TSubclassOf<APlayerStart> PlayerStartClass;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float ZOffset = 50.f;

protected:
	void BuildLevel();
	FTransform AddOffsetinZ(const FTransform& InTransform, float ZVal) const;

private:
	bool bPlayerStartSpawned = false;	


};
