// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TikkiTakaGameMode.generated.h"

/**
 * 
 */
class ATT_TankPawn;
class ATT_TowerPawn;

UCLASS()
class BATTLEBLASTER_API ATikkiTakaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public: 

	void ActorDied(AActor* DeadActor);	

	UFUNCTION(BlueprintCallable, Category = "Towers")
	void RegisterTower(ATT_TowerPawn* Tower);

	UFUNCTION(BlueprintCallable, Category = "Towers")
	void UnregisterTower(ATT_TowerPawn* Tower);

	UFUNCTION(BlueprintPure, Category = "Towers")
	int32 GetActiveTowerCount() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Towers")
	int32 ActiveTowerCount = 0;

protected:
	UPROPERTY()
	TObjectPtr<ATT_TankPawn> PlayerTank = nullptr;

	UPROPERTY()
	TSet<TObjectPtr<ATT_TowerPawn>> ActiveTowers;
	
};
