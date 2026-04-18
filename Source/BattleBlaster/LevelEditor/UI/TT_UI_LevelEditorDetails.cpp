// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_UI_LevelEditorDetails.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "EngineUtils.h"
#include "../Editor/TT_RuntimeLevelEditor.h"

void UTT_UI_LevelEditorDetails::NativeConstruct()
{
	Super::NativeConstruct();

	for (TActorIterator<ATT_RuntimeLevelEditor> It(GetWorld()); It; ++It)
	{
		EditorManager = *It;
		break;
	}
}

float UTT_UI_LevelEditorDetails::ReadFloat(UEditableTextBox* TextBox, float DefaultValue) const
{
	if (!TextBox)
	{
		return DefaultValue;
	}

	const FString Text = TextBox->GetText().ToString();
	if (Text.IsEmpty())
	{
		return DefaultValue;
	}

	return FCString::Atof(*Text);
}

void UTT_UI_LevelEditorDetails::WriteFloat(UEditableTextBox* TextBox, float Value)
{
	if (!TextBox)
	{
		return;
	}

	TextBox->SetText(FText::AsNumber(Value));
}

void UTT_UI_LevelEditorDetails::RefreshFromEditorState()
{
	if (!EditorManager)
	{
		return;
	}

	const FTT_CustomPlacedActor* Record = EditorManager->GetSelectedRecord();
	if (!Record)
	{
		if (SelectedActorText)
		{
			SelectedActorText->SetText(FText::FromString(TEXT("No Selection")));
		}
		return;
	}

	if (SelectedActorText)
	{
		const FString TypeString =
			StaticEnum<ETT_CustomPlaceableType>()->GetNameStringByValue((int64)Record->Type);

		SelectedActorText->SetText(FText::FromString(TypeString));
	}

	const FVector Loc = Record->Transform.GetLocation();
	const FRotator Rot = Record->Transform.Rotator();

	WriteFloat(PosXText, Loc.X);
	WriteFloat(PosYText, Loc.Y);
	WriteFloat(PosZText, Loc.Z);
	WriteFloat(YawText, Rot.Yaw);

	if (Record->Type == ETT_CustomPlaceableType::EnemyTower)
	{
		WriteFloat(TowerScanRangeText, Record->TowerTuning.ScanRange);
		WriteFloat(TowerFireRangeText, Record->TowerTuning.FireRange);
		WriteFloat(TowerFireCooldownText, Record->TowerTuning.FireCooldown);
		WriteFloat(TowerAimSpeedText, Record->TowerTuning.AimSpeed);
		WriteFloat(TowerAimToleranceText, Record->TowerTuning.AimToleranceDegrees);
	}
	else if (Record->Type == ETT_CustomPlaceableType::EnemyTank)
	{
		WriteFloat(TankScanRangeText, Record->TankTuning.ScanRange);
		WriteFloat(TankAttackRangeText, Record->TankTuning.AttackRange);
		WriteFloat(TankFireCooldownText, Record->TankTuning.FireCooldown);
		WriteFloat(TankAimSpeedText, Record->TankTuning.AimSpeed);
		WriteFloat(TankFireToleranceText, Record->TankTuning.FireAngleTolerance);
		WriteFloat(TankLowHealthText, Record->TankTuning.LowHealthThreshold);
		WriteFloat(TankRecoverHealthText, Record->TankTuning.RecoverHealthThreshold);
		WriteFloat(TankPatrolAcceptanceText, Record->TankTuning.PatrolPointAcceptanceRadius);
	}
}

void UTT_UI_LevelEditorDetails::ApplyTransformFromUI()
{
	if (!EditorManager)
	{
		return;
	}

	const FTT_CustomPlacedActor* Record = EditorManager->GetSelectedRecord();
	if (!Record)
	{
		return;
	}

	const FVector NewLocation(
		ReadFloat(PosXText, Record->Transform.GetLocation().X),
		ReadFloat(PosYText, Record->Transform.GetLocation().Y),
		ReadFloat(PosZText, Record->Transform.GetLocation().Z));

	const FRotator NewRotation(
		0.f,
		ReadFloat(YawText, Record->Transform.Rotator().Yaw),
		0.f);

	FTransform NewTransform(NewRotation, NewLocation, Record->Transform.GetScale3D());
	EditorManager->UpdateSelectedTransform(NewTransform);
}

void UTT_UI_LevelEditorDetails::ApplyTowerTuningFromUI()
{
	if (!EditorManager)
	{
		return;
	}

	const FTT_CustomPlacedActor* Record = EditorManager->GetSelectedRecord();
	if (!Record || Record->Type != ETT_CustomPlaceableType::EnemyTower)
	{
		return;
	}

	FTT_TowerTuning NewTuning = Record->TowerTuning;
	NewTuning.ScanRange = ReadFloat(TowerScanRangeText, NewTuning.ScanRange);
	NewTuning.FireRange = ReadFloat(TowerFireRangeText, NewTuning.FireRange);
	NewTuning.FireCooldown = ReadFloat(TowerFireCooldownText, NewTuning.FireCooldown);
	NewTuning.AimSpeed = ReadFloat(TowerAimSpeedText, NewTuning.AimSpeed);
	NewTuning.AimToleranceDegrees = ReadFloat(TowerAimToleranceText, NewTuning.AimToleranceDegrees);

	EditorManager->UpdateSelectedTowerTuning(NewTuning);
}

void UTT_UI_LevelEditorDetails::ApplyTankTuningFromUI()
{
	if (!EditorManager)
	{
		return;
	}

	const FTT_CustomPlacedActor* Record = EditorManager->GetSelectedRecord();
	if (!Record || Record->Type != ETT_CustomPlaceableType::EnemyTank)
	{
		return;
	}

	FTT_TankTuning NewTuning = Record->TankTuning;
	NewTuning.ScanRange = ReadFloat(TankScanRangeText, NewTuning.ScanRange);
	NewTuning.AttackRange = ReadFloat(TankAttackRangeText, NewTuning.AttackRange);
	NewTuning.FireCooldown = ReadFloat(TankFireCooldownText, NewTuning.FireCooldown);
	NewTuning.AimSpeed = ReadFloat(TankAimSpeedText, NewTuning.AimSpeed);
	NewTuning.FireAngleTolerance = ReadFloat(TankFireToleranceText, NewTuning.FireAngleTolerance);
	NewTuning.LowHealthThreshold = ReadFloat(TankLowHealthText, NewTuning.LowHealthThreshold);
	NewTuning.RecoverHealthThreshold = ReadFloat(TankRecoverHealthText, NewTuning.RecoverHealthThreshold);
	NewTuning.PatrolPointAcceptanceRadius = ReadFloat(TankPatrolAcceptanceText, NewTuning.PatrolPointAcceptanceRadius);

	EditorManager->UpdateSelectedTankTuning(NewTuning);
}

void UTT_UI_LevelEditorDetails::ToggleSelectedTankPatrolPoint(FName PatrolPointId, bool bShouldLink)
{
	if (!EditorManager)
	{
		return;
	}

	if (bShouldLink)
	{
		EditorManager->LinkSelectedTankToPatrolPoint(PatrolPointId);
	}
	else
	{
		EditorManager->UnlinkSelectedTankFromPatrolPoint(PatrolPointId);
	}
}

bool UTT_UI_LevelEditorDetails::IsPatrolPointLinkedForSelectedTank(FName PatrolPointId) const
{
	if (!EditorManager)
	{
		return false;
	}

	const FTT_CustomPlacedActor* Record = EditorManager->GetSelectedRecord();
	if (!Record || Record->Type != ETT_CustomPlaceableType::EnemyTank)
	{
		return false;
	}

	return Record->LinkedPatrolPointIds.Contains(PatrolPointId);
}