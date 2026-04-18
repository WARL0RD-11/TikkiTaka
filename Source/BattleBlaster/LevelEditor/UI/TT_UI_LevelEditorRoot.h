// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TT_UI_LevelEditorRoot.generated.h"

/**
 * 
 */
class ATT_RuntimeLevelEditor;
class UEditableTextBox;
class UTextBlock;
class UTT_UI_LevelEditorDetails;

UCLASS()
class BATTLEBLASTER_API UTT_UI_LevelEditorRoot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	ATT_RuntimeLevelEditor* EditorManager = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* CurrentToolText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* ValidationText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* SelectedTypeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* LevelNameTextBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UTT_UI_LevelEditorDetails* DetailsPanel;

	UFUNCTION(BlueprintCallable)
	void OnPlacePlayerStart();

	UFUNCTION(BlueprintCallable)
	void OnPlaceEnemyTower();

	UFUNCTION(BlueprintCallable)
	void OnPlaceEnemyTank();

	UFUNCTION(BlueprintCallable)
	void OnPlaceWall();

	UFUNCTION(BlueprintCallable)
	void OnPlacePatrolPoint();

	UFUNCTION(BlueprintCallable)
	void OnDeleteSelected();

	UFUNCTION(BlueprintCallable)
	void OnSavePressed();

	UFUNCTION(BlueprintCallable)
	void OnLoadPressed();

	UFUNCTION(BlueprintCallable)
	void OnPlayNowPressed();

	UFUNCTION(BlueprintCallable)
	void OnAddToCampaignPressed();

	UFUNCTION(BlueprintCallable)
	void OnClearSelection();

	void RefreshStatusText();
	
};
