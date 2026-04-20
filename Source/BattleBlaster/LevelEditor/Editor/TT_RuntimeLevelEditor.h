// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Data/TT_CustomLevelTypes.h"
#include "TT_RuntimeLevelEditor.generated.h"

class ATT_EnemyTank;
class ATT_TowerPawn;
class UTT_CustomLevelSaveGame;
class UTT_UI_LevelEditorRoot;

UCLASS()
class BATTLEBLASTER_API ATT_RuntimeLevelEditor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATT_RuntimeLevelEditor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// ---------- Preview classes ----------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Preview")
	TSubclassOf<AActor> EnemyTankPreviewClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Preview")
	TSubclassOf<AActor> EnemyTowerPreviewClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Preview")
	TSubclassOf<AActor> WallPreviewClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Preview")
	TSubclassOf<AActor> PatrolPointPreviewClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Preview")
	TSubclassOf<AActor> PlayerStartPreviewClass;

	// ---------- UI ----------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|UI")
	TSubclassOf<UTT_UI_LevelEditorRoot> LevelEditorWidgetClass;

	// ---------- Placement ----------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Placement")
	FVector PlacementBoundsMin = FVector(-5000.f, -5000.f, -100.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Placement")
	FVector PlacementBoundsMax = FVector(5000.f, 5000.f, 500.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Placement")
	float GridSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Placement")
	float OverlapCheckRadius = 125.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Placement")
	float FixedPlacementZ = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Editor|Placement")
	bool bLockPlacementToFixedZ = true;

	// ---------- State ----------
	UPROPERTY(BlueprintReadOnly, Category = "Editor|State")
	ETT_CustomPlaceableType CurrentPlacementType = ETT_CustomPlaceableType::None;

	UPROPERTY(BlueprintReadOnly, Category = "Editor|State")
	TArray<FTT_CustomPlacedActor> WorkingActors;

	UPROPERTY(BlueprintReadOnly, Category = "Editor|State")
	FName SelectedInstanceId = NAME_None;

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void SetCurrentPlacementType(ETT_CustomPlaceableType NewType);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool PlaceAtWorldLocation(const FVector& WorldLocation, const FRotator& WorldRotation);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool SelectByPreviewActor(AActor* PreviewActor);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool DeleteSelected();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool UpdateSelectedTransform(const FTransform& NewTransform);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool UpdateSelectedTowerTuning(const FTT_TowerTuning& NewTuning);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool UpdateSelectedTankTuning(const FTT_TankTuning& NewTuning);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool LinkSelectedTankToPatrolPoint(FName PatrolPointId);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool UnlinkSelectedTankFromPatrolPoint(FName PatrolPointId);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool SaveLevel(const FString& LevelName, bool bAddToCampaign);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool LoadLevel();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool ValidateLevel(FString& OutError) const;

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void PlayNow();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void ClearCurrentLevel();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool DeleteCurrentLevel();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool IsGhostPlacementValid() const { return bGhostPlacementValid; }

	UFUNCTION(BlueprintCallable, Category = "Editor")
	FTransform GetGhostTransform() const { return GhostTransform; }

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void UpdateGhostFromWorldLocation(const FVector& WorldLocation, const FRotator& DesiredRotation);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	bool HasSelectedRecord() const { return !SelectedInstanceId.IsNone(); }


	const FTT_CustomPlacedActor* GetSelectedRecord() const;

	UFUNCTION(BlueprintCallable, Category = "Editor")
	TArray<FTT_CustomPlacedActor> GetPatrolPointRecords() const;

	UFUNCTION(BlueprintCallable, Category = "Editor")
	FString GetSelectedTypeAsString() const;

	bool BeginPatrolLinkForSelectedTank();
	void ClearPatrolLinkTarget();
	bool LinkPatrolPointToTank(FName TankInstanceId, FName PatrolPointId);
	bool UnlinkPatrolPointFromTank(FName TankInstanceId, FName PatrolPointId);

	UFUNCTION(BlueprintPure, Category = "Level Editor|Patrol Linking")
	bool HasPatrolLinkTarget() const { return !PatrolLinkTankInstanceId.IsNone(); }

	UFUNCTION(BlueprintPure, Category = "Level Editor|Patrol Linking")
	FName GetPatrolLinkTankInstanceId() const { return PatrolLinkTankInstanceId; }

protected:
	UPROPERTY()
	TMap<FName, AActor*> PreviewActorMap;

	UPROPERTY()
	AActor* GhostPreviewActor = nullptr;

	UPROPERTY()
	UTT_UI_LevelEditorRoot* LevelEditorWidget = nullptr;

	UPROPERTY()
	bool bGhostPlacementValid = false;

	UPROPERTY()
	FTransform GhostTransform = FTransform::Identity;

	protected:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Editor|Patrol Linking")
		FName PatrolLinkTankInstanceId = NAME_None;

protected:
	void CreateEditorWidget();

	FName GenerateInstanceId(ETT_CustomPlaceableType Type) const;
	FVector SnapLocationToGrid(const FVector& WorldLocation) const;
	bool IsWithinPlacementBounds(const FVector& Location) const;

	bool IsPlacementLocationValid(
		ETT_CustomPlaceableType Type,
		const FVector& Location,
		FString& OutError,
		FName IgnoreInstanceId = NAME_None) const;

	TSubclassOf<AActor> ResolvePreviewClass(ETT_CustomPlaceableType Type) const;
	AActor* SpawnPreviewForRecord(const FTT_CustomPlacedActor& Record);
	void RebuildAllPreviewActors();

	FTT_CustomPlacedActor* FindSelectedRecord();
	const FTT_CustomPlacedActor* FindSelectedRecordConst() const;

	void RefreshGhostPreview();
	void DestroyGhostPreview();
	void UpdateGhostVisualState();

	void SetPreviewHighlight(AActor* Actor, bool bHighlighted);

private:	

};
