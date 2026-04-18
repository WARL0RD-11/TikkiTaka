// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TT_GameInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETT_PlayFlowMode : uint8
{
	Campaign,
	CustomPreview,
	CampaignThenCustom
};

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

	UPROPERTY(BlueprintReadWrite, Category = "CustomLevel")
	ETT_PlayFlowMode PlayFlowMode = ETT_PlayFlowMode::Campaign;

	UPROPERTY(BlueprintReadWrite, Category = "CustomLevel")
	bool bHasSavedCustomLevel = false;

	UPROPERTY(BlueprintReadWrite, Category = "CustomLevel")
	bool bCustomLevelAddedToProgression = false;

	UFUNCTION(BlueprintCallable)
	void OpenLevelEditor();

	UFUNCTION(BlueprintCallable)
	void PlayCustomLevelNow();

	UFUNCTION(BlueprintCallable)
	void SetCustomLevelAddedToProgression(bool bEnabled);

private:
	void ChangeLevelIndex(int32 Index);	
};
