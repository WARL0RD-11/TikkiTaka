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

	FTimerHandle CountdownTimerHandle;

	UFUNCTION()
	void CountdownTimerTick();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Towers")
	int32 ActiveTowerCount = 0;

	UPROPERTY()
	TObjectPtr<ATT_TankPawn> PlayerTank = nullptr;

	UPROPERTY()
	TSet<TObjectPtr<ATT_TowerPawn>> ActiveTowers;

	UPROPERTY(EditDefaultsOnly, Category = "Restart Delay")
	float RestartDelay = 3.f;	

	FTimerHandle RestartTimerHandle;	

	UFUNCTION()
	void GameLevelTransition();

	UPROPERTY()
	bool bIsVictory = false;

	UPROPERTY(EditAnywhere)
	int32 LevelStartDelay = 3.f;

	int32 Countdown;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UTT_UI_ScreenMsg> ScreenMsgWidgetClass;

private:
	class ATT_PlayerController* PC = nullptr;

	UPROPERTY()
	TObjectPtr<UTT_UI_ScreenMsg> ScreenMsgWidget;	
	
};
