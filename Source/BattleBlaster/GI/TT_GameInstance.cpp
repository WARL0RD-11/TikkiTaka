#include "TT_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../LevelEditor/Save/TT_CustomLevelSaveGame.h"

static const FString GCustomLevelSlotName = TEXT("CustomLevelSlot");
static const int32 GCustomLevelSlotIndex = 0;

void UTT_GameInstance::Init()
{
	Super::Init();
	RefreshCustomLevelStateFromSave();
}

void UTT_GameInstance::RefreshCustomLevelStateFromSave()
{
	bHasSavedCustomLevel = false;
	bCustomLevelAddedToProgression = false;

	if (!UGameplayStatics::DoesSaveGameExist(GCustomLevelSlotName, GCustomLevelSlotIndex))
	{
		PlayFlowMode = ETT_PlayFlowMode::Campaign;

		UE_LOG(LogTemp, Warning, TEXT("No custom level save found. Campaign will run without custom level."));
		return;
	}

	UTT_CustomLevelSaveGame* SaveObj = Cast<UTT_CustomLevelSaveGame>(
		UGameplayStatics::LoadGameFromSlot(GCustomLevelSlotName, GCustomLevelSlotIndex));

	if (!SaveObj)
	{
		PlayFlowMode = ETT_PlayFlowMode::Campaign;

		UE_LOG(LogTemp, Warning, TEXT("Failed to load custom level save. Campaign will run without custom level."));
		return;
	}

	bHasSavedCustomLevel = true;
	bCustomLevelAddedToProgression = SaveObj->SavedLevel.bAddToCampaignProgression;

	PlayFlowMode = (bHasSavedCustomLevel && bCustomLevelAddedToProgression)
		? ETT_PlayFlowMode::CampaignThenCustom
		: ETT_PlayFlowMode::Campaign;

	UE_LOG(LogTemp, Warning, TEXT("RefreshCustomLevelStateFromSave: HasSave=%s AddToCampaign=%s PlayFlowMode=%d"),
		bHasSavedCustomLevel ? TEXT("true") : TEXT("false"),
		bCustomLevelAddedToProgression ? TEXT("true") : TEXT("false"),
		(int32)PlayFlowMode);
}

void UTT_GameInstance::OpenLevelEditor()
{
	UGameplayStatics::OpenLevel(this, TEXT("L_LevelEditor"));
}

void UTT_GameInstance::PlayCustomLevelNow()
{
	// Manual play from editor / preview path
	PlayFlowMode = ETT_PlayFlowMode::CustomPreview;
	UGameplayStatics::OpenLevel(this, TEXT("L_CustomRuntime"));
}

void UTT_GameInstance::SetCustomLevelAddedToProgression(bool bEnabled)
{
	bCustomLevelAddedToProgression = bEnabled;

	PlayFlowMode = (bHasSavedCustomLevel && bEnabled)
		? ETT_PlayFlowMode::CampaignThenCustom
		: ETT_PlayFlowMode::Campaign;
}

void UTT_GameInstance::ChangeLevelIndex(int32 Index)
{
	if (Index >= 1 && Index <= LastLevelIndex)
	{
		CurrentLevelIndex = Index;

		const FString LevelName = FString::Printf(TEXT("Level_%d"), CurrentLevelIndex);
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
		GoToEndScreen();
		return;
	}

	RefreshCustomLevelStateFromSave();

	UE_LOG(LogTemp, Warning, TEXT("LoadNextLevel: CurrentLevelIndex=%d LastLevelIndex=%d HasSave=%s AddToCampaign=%s PlayFlowMode=%d"),
		CurrentLevelIndex,
		LastLevelIndex,
		bHasSavedCustomLevel ? TEXT("true") : TEXT("false"),
		bCustomLevelAddedToProgression ? TEXT("true") : TEXT("false"),
		(int32)PlayFlowMode);

	if (CurrentLevelIndex < LastLevelIndex)
	{
		ChangeLevelIndex(CurrentLevelIndex + 1);
		return;
	}

	if (CurrentLevelIndex == LastLevelIndex)
	{
		if (bHasSavedCustomLevel && bCustomLevelAddedToProgression)
		{
			PlayFlowMode = ETT_PlayFlowMode::CampaignThenCustom;
			UGameplayStatics::OpenLevel(this, TEXT("L_CustomRuntime"));
			return;
		}

		GoToEndScreen();
		return;
	}

	GoToEndScreen();
}

void UTT_GameInstance::RestartCurrentLevel()
{
	if (PlayFlowMode == ETT_PlayFlowMode::CustomPreview)
	{
		UGameplayStatics::OpenLevel(this, TEXT("L_CustomRuntime"));
		return;
	}

	if (CurrentLevelIndex >= 1 && CurrentLevelIndex <= LastLevelIndex)
	{
		ChangeLevelIndex(CurrentLevelIndex);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("RestartCurrentLevel failed: CurrentLevelIndex is invalid."));
}

void UTT_GameInstance::RestartGame()
{
	RefreshCustomLevelStateFromSave();

	CurrentLevelIndex = 1;

	// Restarting game should return to campaign from the start
	PlayFlowMode = (bHasSavedCustomLevel && bCustomLevelAddedToProgression)
		? ETT_PlayFlowMode::CampaignThenCustom
		: ETT_PlayFlowMode::Campaign;

	ChangeLevelIndex(CurrentLevelIndex);
}

void UTT_GameInstance::GoToEndScreen()
{
	UGameplayStatics::OpenLevel(this, TEXT("L_EndScreen"));
}