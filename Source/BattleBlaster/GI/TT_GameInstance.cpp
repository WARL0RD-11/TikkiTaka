// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UTT_GameInstance::ChangeLevelIndex(int32 Index)
{
	if (Index > 0 && Index <= LastLevelIndex)
	{
		CurrentLevelIndex = Index;

		FString LevelName = FString::Printf(TEXT("Level_%d"), CurrentLevelIndex);	
		UGameplayStatics::OpenLevel(this, *LevelName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid level index: %d. Valid range is 1 to %d."), Index, LastLevelIndex);
	}
}

void UTT_GameInstance::LoadNextLevel()
{
	if (CurrentLevelIndex < LastLevelIndex)
	{
		ChangeLevelIndex(CurrentLevelIndex + 1);
	}
	else
	{
		RestartGame();
	}
}

void UTT_GameInstance::RestartCurrentLevel()
{
	ChangeLevelIndex(CurrentLevelIndex);
}

void UTT_GameInstance::RestartGame()
{
	if (CurrentLevelIndex == LastLevelIndex)
	{
		CurrentLevelIndex = 1;
		ChangeLevelIndex(CurrentLevelIndex);
	}
}