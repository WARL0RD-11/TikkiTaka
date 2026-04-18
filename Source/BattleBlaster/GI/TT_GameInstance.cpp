// Game Instance


#include "TT_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UTT_GameInstance::OpenLevelEditor()
{
	UGameplayStatics::OpenLevel(this, TEXT("L_LevelEditor"));
}

void UTT_GameInstance::PlayCustomLevelNow()
{
	UGameplayStatics::OpenLevel(this, TEXT("L_CustomRuntime"));
}

void UTT_GameInstance::SetCustomLevelAddedToProgression(bool bEnabled)
{
	bHasSavedCustomLevel = true;
	bCustomLevelAddedToProgression = bEnabled;
	PlayFlowMode = bEnabled ? ETT_PlayFlowMode::CampaignThenCustom : ETT_PlayFlowMode::Campaign;
}

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
	if (PlayFlowMode == ETT_PlayFlowMode::CustomPreview)
	{
		RestartGame();
		return;
	}

	if (CurrentLevelIndex < LastLevelIndex)
	{
		ChangeLevelIndex(CurrentLevelIndex + 1);
		return;
	}

	if (PlayFlowMode == ETT_PlayFlowMode::CampaignThenCustom &&
		bHasSavedCustomLevel &&
		bCustomLevelAddedToProgression)
	{
		UGameplayStatics::OpenLevel(this, TEXT("L_CustomRuntime"));
		return;
	}

	RestartGame();
}

void UTT_GameInstance::RestartCurrentLevel()
{
	if (PlayFlowMode == ETT_PlayFlowMode::CustomPreview)
	{
		UGameplayStatics::OpenLevel(this, TEXT("L_CustomRuntime"));
		return;
	}

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