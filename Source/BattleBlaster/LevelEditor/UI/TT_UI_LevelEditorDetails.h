// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Data/TT_CustomLevelTypes.h"
#include "TT_UI_LevelEditorDetails.generated.h"

/**
 * 
 */
class ATT_RuntimeLevelEditor;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class BATTLEBLASTER_API UTT_UI_LevelEditorDetails : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void RefreshFromEditorState();

	UFUNCTION(BlueprintCallable)
	void ApplyTransformFromUI();

	UFUNCTION(BlueprintCallable)
	void ApplyTowerTuningFromUI();

	UFUNCTION(BlueprintCallable)
	void ApplyTankTuningFromUI();

	UFUNCTION(BlueprintCallable)
	void ToggleSelectedTankPatrolPoint(FName PatrolPointId, bool bShouldLink);

	UFUNCTION(BlueprintCallable)
	bool IsPatrolPointLinkedForSelectedTank(FName PatrolPointId) const;

	UFUNCTION(BlueprintCallable)
	FName GetSelectedInstanceId() const;

	UFUNCTION(BlueprintCallable)
	ETT_CustomPlaceableType GetSelectedPlaceableType() const;

	UFUNCTION(BlueprintCallable)
	TArray<FTT_CustomPlacedActor> GetAvailablePatrolPointRecords() const;

	UPROPERTY()
	FName LastRefreshedSelectionId = NAME_None;

	UFUNCTION(BlueprintCallable)
	void ForceRefreshFromEditorState();

protected:
	UPROPERTY(BlueprintReadOnly)
	ATT_RuntimeLevelEditor* EditorManager = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* SelectedActorText;

	// Transform
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* PosXText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* PosYText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* PosZText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* YawText;

	// Tower
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TowerScanRangeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TowerFireRangeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TowerFireCooldownText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TowerAimSpeedText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TowerAimToleranceText;

	// Tank
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankScanRangeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankAttackRangeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankFireCooldownText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankAimSpeedText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankFireToleranceText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankLowHealthText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankRecoverHealthText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	UEditableTextBox* TankPatrolAcceptanceText;

protected:
	float ReadFloat(UEditableTextBox* TextBox, float DefaultValue) const;
	void WriteFloat(UEditableTextBox* TextBox, float Value);
};
