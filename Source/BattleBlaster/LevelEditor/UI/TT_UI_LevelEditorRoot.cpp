// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_UI_LevelEditorRoot.h"

#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "EngineUtils.h"

#include "../Editor/TT_RuntimeLevelEditor.h"
#include "TT_UI_LevelEditorDetails.h"

void UTT_UI_LevelEditorRoot::NativeConstruct()
{
	Super::NativeConstruct();

	for (TActorIterator<ATT_RuntimeLevelEditor> It(GetWorld()); It; ++It)
	{
		EditorManager = *It;
		break;
	}

	if (DetailsPanel)
	{
		DetailsPanel->ForceRefreshFromEditorState();
	}

	RefreshStatusText();
}

void UTT_UI_LevelEditorRoot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshStatusText();
}

void UTT_UI_LevelEditorRoot::RefreshStatusText()
{
	if (!EditorManager)
	{
		return;
	}

	if (CurrentToolText)
	{
		const FString ToolName =
			StaticEnum<ETT_CustomPlaceableType>()->GetNameStringByValue((int64)EditorManager->CurrentPlacementType);

		CurrentToolText->SetText(FText::FromString(FString::Printf(TEXT("Tool: %s"), *ToolName)));
	}

	if (ValidationText)
	{
		FString ValidationString = EditorManager->IsGhostPlacementValid()
			? TEXT("Placement: Valid")
			: TEXT("Placement: Invalid");

		FString SaveError;
		if (!EditorManager->ValidateLevel(SaveError))
		{
			ValidationString += FString::Printf(TEXT(" | Save Check: %s"), *SaveError);
		}
		else
		{
			ValidationString += TEXT(" | Save Check: Ready");
		}

		ValidationText->SetText(FText::FromString(ValidationString));
	}

	if (SelectedTypeText)
	{
		SelectedTypeText->SetText(FText::FromString(
			FString::Printf(TEXT("Selected: %s"), *EditorManager->GetSelectedTypeAsString())));
	}
}

void UTT_UI_LevelEditorRoot::OnClearLevel()
{
	if (EditorManager) EditorManager->ClearCurrentLevel();
}

void UTT_UI_LevelEditorRoot::OnDeleteLevel()
{
	if (EditorManager) EditorManager->DeleteCurrentLevel();
}

void UTT_UI_LevelEditorRoot::OnPlacePlayerStart()
{
	if (EditorManager) EditorManager->SetCurrentPlacementType(ETT_CustomPlaceableType::PlayerStart);
}

void UTT_UI_LevelEditorRoot::OnPlaceEnemyTower()
{
	if (EditorManager) EditorManager->SetCurrentPlacementType(ETT_CustomPlaceableType::EnemyTower);
}

void UTT_UI_LevelEditorRoot::OnPlaceEnemyTank()
{
	if (EditorManager) EditorManager->SetCurrentPlacementType(ETT_CustomPlaceableType::EnemyTank);
}

void UTT_UI_LevelEditorRoot::OnPlaceWall()
{
	if (EditorManager) EditorManager->SetCurrentPlacementType(ETT_CustomPlaceableType::Wall);
}

void UTT_UI_LevelEditorRoot::OnPlacePatrolPoint()
{
	if (EditorManager) EditorManager->SetCurrentPlacementType(ETT_CustomPlaceableType::PatrolPoint);
}

void UTT_UI_LevelEditorRoot::OnDeleteSelected()
{
	if (EditorManager)
	{
		EditorManager->DeleteSelected();
	}
}

void UTT_UI_LevelEditorRoot::OnSavePressed()
{
	if (!EditorManager)
	{
		return;
	}

	const FString LevelName = LevelNameTextBox
		? LevelNameTextBox->GetText().ToString()
		: TEXT("Custom Level");

	EditorManager->SaveLevel(LevelName, false);
}

void UTT_UI_LevelEditorRoot::OnLoadPressed()
{
	if (EditorManager)
	{
		EditorManager->LoadLevel();

		if (DetailsPanel)
		{
			DetailsPanel->ForceRefreshFromEditorState();
		}
	}
}

void UTT_UI_LevelEditorRoot::OnPlayNowPressed()
{
	if (!EditorManager)
	{
		return;
	}

	const FString LevelName = LevelNameTextBox
		? LevelNameTextBox->GetText().ToString()
		: TEXT("Custom Level");

	if (EditorManager->SaveLevel(LevelName, false))
	{
		EditorManager->PlayNow();
	}
}

void UTT_UI_LevelEditorRoot::OnAddToCampaignPressed()
{
	if (!EditorManager)
	{
		return;
	}

	const FString LevelName = LevelNameTextBox
		? LevelNameTextBox->GetText().ToString()
		: TEXT("Custom Level");

	EditorManager->SaveLevel(LevelName, true);
}

void UTT_UI_LevelEditorRoot::OnClearSelection()
{
	if (EditorManager)
	{
		EditorManager->ClearSelection();

		if (DetailsPanel)
		{
			DetailsPanel->ForceRefreshFromEditorState();
		}
	}
}

void UTT_UI_LevelEditorRoot::RefreshDetailsPanel()
{
	if (DetailsPanel)
	{
		DetailsPanel->ForceRefreshFromEditorState();
	}
}
