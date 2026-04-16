// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TT_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API UTT_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 LastLevelIndex = 3; //Maximum Levels in the game


	UPROPERTY(VisibleAnywhere)
	int32 CurrentLevelIndex = 1;

	void LoadNextLevel();
	void RestartCurrentLevel();
	void RestartGame();

private:
	void ChangeLevelIndex(int32 Index);	
};
