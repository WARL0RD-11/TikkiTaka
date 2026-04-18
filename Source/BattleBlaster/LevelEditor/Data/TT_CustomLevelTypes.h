// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_CustomLevelTypes.generated.h"

/**
 * 
*/

UENUM(BlueprintType)
enum class ETT_CustomPlaceableType : uint8
{
	None, 
	PlayerStart,	
	EnemyTower,
	EnemyTank,
	Wall,
	PatrolPoint,
};

USTRUCT(BlueprintType)
struct FTT_TowerTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float ScanRange = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float FireRange = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float FireCooldown = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float AimSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float AimToleranceDegrees = 6.f;
};

USTRUCT(BlueprintType)
struct FTT_TankTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float ScanRange = 3500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float AttackRange = 1800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float FireCooldown = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float AimSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float FireAngleTolerance = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float LowHealthThreshold = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float RecoverHealthThreshold = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	float PatrolPointAcceptanceRadius = 150.f;
};

USTRUCT(BlueprintType)
struct FTT_CustomPlacedActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placed")
	FName InstanceId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placed")
	ETT_CustomPlaceableType Type = ETT_CustomPlaceableType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placed")
	FTransform Transform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placed")
	TArray<FName> LinkedPatrolPointIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placed")
	FTT_TowerTuning TowerTuning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placed")
	FTT_TankTuning TankTuning;
};

USTRUCT(BlueprintType)
struct FTT_CustomLevelDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FString LevelDisplayName = TEXT("Custom Level");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<FTT_CustomPlacedActor> PlacedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	bool bAddToCampaignProgression = false;
};