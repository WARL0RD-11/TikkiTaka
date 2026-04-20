
#include "TT_RuntimeLevelEditor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "../Save/TT_CustomLevelSaveGame.h"
#include "BattleBlaster/GI/TT_GameInstance.h"
#include "BattleBlaster/Pawn/Tank/TT_EnemyTank.h"
#include "BattleBlaster/Pawn/Tower/TT_TowerPawn.h"
#include "../UI/TT_UI_LevelEditorRoot.h"

static const FString GCustomLevelSlotName = TEXT("CustomLevelSlot");
static const int32 GCustomLevelSlotIndex = 0;

ATT_RuntimeLevelEditor::ATT_RuntimeLevelEditor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATT_RuntimeLevelEditor::BeginPlay()
{
	Super::BeginPlay();

	CreateEditorWidget();
	LoadLevel();
	RefreshGhostPreview();
}

void ATT_RuntimeLevelEditor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateGhostVisualState();
}

void ATT_RuntimeLevelEditor::CreateEditorWidget()
{
	if (!LevelEditorWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelEditorWidgetClass is not set on RuntimeLevelEditorManager."));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerController found for Level Editor UI creation."));
		return;
	}

	LevelEditorWidget = CreateWidget<UTT_UI_LevelEditorRoot>(PC, LevelEditorWidgetClass);
	if (LevelEditorWidget)
	{
		LevelEditorWidget->AddToViewport();
	}
}

void ATT_RuntimeLevelEditor::SetCurrentPlacementType(ETT_CustomPlaceableType NewType)
{
	CurrentPlacementType = NewType;
	RefreshGhostPreview();
}

FName ATT_RuntimeLevelEditor::GenerateInstanceId(ETT_CustomPlaceableType Type) const
{
	const FString TypeString = StaticEnum<ETT_CustomPlaceableType>()->GetNameStringByValue((int64)Type);
	return FName(*FString::Printf(TEXT("%s_%d"), *TypeString, FMath::RandRange(100000, 999999)));
}

FVector ATT_RuntimeLevelEditor::SnapLocationToGrid(const FVector& WorldLocation) const
{
	FVector Result = WorldLocation;
	Result.X = FMath::GridSnap(Result.X, GridSize);
	Result.Y = FMath::GridSnap(Result.Y, GridSize);

	if (bLockPlacementToFixedZ)
	{
		Result.Z = FixedPlacementZ;
	}

	return Result;
}

bool ATT_RuntimeLevelEditor::IsWithinPlacementBounds(const FVector& Location) const
{
	return
		Location.X >= PlacementBoundsMin.X && Location.X <= PlacementBoundsMax.X &&
		Location.Y >= PlacementBoundsMin.Y && Location.Y <= PlacementBoundsMax.Y &&
		Location.Z >= PlacementBoundsMin.Z && Location.Z <= PlacementBoundsMax.Z;
}

bool ATT_RuntimeLevelEditor::IsPlacementLocationValid(
	ETT_CustomPlaceableType Type,
	const FVector& Location,
	FString& OutError,
	FName IgnoreInstanceId) const
{
	if (Type == ETT_CustomPlaceableType::None)
	{
		OutError = TEXT("No placement type selected.");
		return false;
	}

	if (!IsWithinPlacementBounds(Location))
	{
		OutError = TEXT("Location is outside editable bounds.");
		return false;
	}

	// Only one player start
	if (Type == ETT_CustomPlaceableType::PlayerStart)
	{
		for (const FTT_CustomPlacedActor& Record : WorkingActors)
		{
			if (Record.InstanceId == IgnoreInstanceId)
			{
				continue;
			}

			if (Record.Type == ETT_CustomPlaceableType::PlayerStart)
			{
				OutError = TEXT("Only one PlayerStart is allowed.");
				return false;
			}
		}
	}

	for (const FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.InstanceId == IgnoreInstanceId)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(Record.Transform.GetLocation(), Location);

		const float RequiredSpacing =
			(Type == ETT_CustomPlaceableType::Wall || Record.Type == ETT_CustomPlaceableType::Wall)
			? OverlapCheckRadius * 2.f
			: OverlapCheckRadius;

		if (DistSq <= FMath::Square(RequiredSpacing))
		{
			OutError = TEXT("Too close to another placed object.");
			return false;
		}
	}

	return true;
}

TSubclassOf<AActor> ATT_RuntimeLevelEditor::ResolvePreviewClass(ETT_CustomPlaceableType Type) const
{
	switch (Type)
	{
	case ETT_CustomPlaceableType::PlayerStart:
		return PlayerStartPreviewClass;
	case ETT_CustomPlaceableType::EnemyTower:
		return EnemyTowerPreviewClass;
	case ETT_CustomPlaceableType::EnemyTank:
		return EnemyTankPreviewClass;
	case ETT_CustomPlaceableType::Wall:
		return WallPreviewClass;
	case ETT_CustomPlaceableType::PatrolPoint:
		return PatrolPointPreviewClass;
	default:
		return nullptr;
	}
}

void ATT_RuntimeLevelEditor::SetPreviewHighlight(AActor* Actor, bool bHighlighted)
{
	if (!IsValid(Actor))
	{
		return;
	}

	TArray<UActorComponent*> Components;
	Actor->GetComponents(UPrimitiveComponent::StaticClass(), Components);
	for (UActorComponent* Comp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
		{
			Prim->SetRenderCustomDepth(bHighlighted);
			Prim->SetCustomDepthStencilValue(bHighlighted ? 1 : 0);
		}
	}
}

AActor* ATT_RuntimeLevelEditor::SpawnPreviewForRecord(const FTT_CustomPlacedActor& Record)
{
	TSubclassOf<AActor> PreviewClass = ResolvePreviewClass(Record.Type);
	if (!PreviewClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No preview class set for placed actor type."));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(PreviewClass, Record.Transform, Params);
	if (!Spawned)
	{
		return nullptr;
	}

	Spawned->Tags.Add(Record.InstanceId);
	Spawned->SetActorTickEnabled(false);

	// IMPORTANT:
	// Preview actors must be traceable by mouse clicks, but should not act like real gameplay collision.
	TArray<UActorComponent*> Components;
	Spawned->GetComponents(UPrimitiveComponent::StaticClass(), Components);
	for (UActorComponent* Comp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
		{
			Prim->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Prim->SetGenerateOverlapEvents(false);
			Prim->SetCollisionResponseToAllChannels(ECR_Ignore);
			Prim->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
	}

	PreviewActorMap.Add(Record.InstanceId, Spawned);
	return Spawned;
}

void ATT_RuntimeLevelEditor::RebuildAllPreviewActors()
{
	for (TPair<FName, AActor*>& Pair : PreviewActorMap)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->Destroy();
		}
	}
	PreviewActorMap.Empty();

	for (const FTT_CustomPlacedActor& Record : WorkingActors)
	{
		SpawnPreviewForRecord(Record);
	}

	// Restore selection highlight if still valid
	if (AActor** Found = PreviewActorMap.Find(SelectedInstanceId))
	{
		SetPreviewHighlight(*Found, true);
	}
}

void ATT_RuntimeLevelEditor::DestroyGhostPreview()
{
	if (IsValid(GhostPreviewActor))
	{
		GhostPreviewActor->Destroy();
		GhostPreviewActor = nullptr;
	}
}

void ATT_RuntimeLevelEditor::RefreshGhostPreview()
{
	DestroyGhostPreview();

	if (CurrentPlacementType == ETT_CustomPlaceableType::None)
	{
		return;
	}

	TSubclassOf<AActor> PreviewClass = ResolvePreviewClass(CurrentPlacementType);
	if (!PreviewClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No preview class set for ghost preview."));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GhostPreviewActor = GetWorld()->SpawnActor<AActor>(
		PreviewClass,
		FTransform::Identity,
		Params);

	if (!GhostPreviewActor)
	{
		return;
	}

	GhostPreviewActor->SetActorTickEnabled(false);

	TArray<UActorComponent*> Components;
	GhostPreviewActor->GetComponents(UPrimitiveComponent::StaticClass(), Components);
	for (UActorComponent* Comp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
		{
			Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Prim->SetGenerateOverlapEvents(false);
			Prim->SetRenderCustomDepth(true);
			Prim->SetCustomDepthStencilValue(2);
		}
	}
}

void ATT_RuntimeLevelEditor::UpdateGhostFromWorldLocation(const FVector& WorldLocation, const FRotator& DesiredRotation)
{
	if (CurrentPlacementType == ETT_CustomPlaceableType::None || !GhostPreviewActor)
	{
		return;
	}

	const FVector Snapped = SnapLocationToGrid(WorldLocation);
	GhostTransform = FTransform(DesiredRotation, Snapped);

	FString Error;
	bGhostPlacementValid = IsPlacementLocationValid(CurrentPlacementType, Snapped, Error);

	GhostPreviewActor->SetActorTransform(GhostTransform);
}

void ATT_RuntimeLevelEditor::UpdateGhostVisualState()
{
	if (!GhostPreviewActor)
	{
		return;
	}

	const FVector DesiredScale = bGhostPlacementValid
		? FVector(1.f, 1.f, 1.f)
		: FVector(1.f, 1.f, 0.96f);

	GhostPreviewActor->SetActorScale3D(DesiredScale);
}

bool ATT_RuntimeLevelEditor::PlaceAtWorldLocation(const FVector& WorldLocation, const FRotator& WorldRotation)
{
	const FVector SnappedLocation = SnapLocationToGrid(WorldLocation);

	FString Error;
	if (!IsPlacementLocationValid(CurrentPlacementType, SnappedLocation, Error))
	{
		UE_LOG(LogTemp, Warning, TEXT("Placement failed: %s"), *Error);
		return false;
	}

	FTT_CustomPlacedActor NewRecord;
	NewRecord.InstanceId = GenerateInstanceId(CurrentPlacementType);
	NewRecord.Type = CurrentPlacementType;
	NewRecord.Transform = FTransform(WorldRotation, SnappedLocation);

	AActor* NewPreview = SpawnPreviewForRecord(NewRecord);
	if (!IsValid(NewPreview))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn preview for placed actor."));
		return false;
	}

	WorkingActors.Add(NewRecord);

	if (AActor** OldSelected = PreviewActorMap.Find(SelectedInstanceId))
	{
		SetPreviewHighlight(*OldSelected, false);
	}

	SelectedInstanceId = NewRecord.InstanceId;
	SetPreviewHighlight(NewPreview, true);

	if (GhostPreviewActor)
	{
		GhostPreviewActor->SetActorTransform(NewRecord.Transform);
	}

	if (LevelEditorWidget)
	{
		LevelEditorWidget->RefreshDetailsPanel();
	}

	return true;
}

bool ATT_RuntimeLevelEditor::SelectByPreviewActor(AActor* PreviewActor)
{
	if (!IsValid(PreviewActor))
	{
		return false;
	}

	if (AActor** OldSelected = PreviewActorMap.Find(SelectedInstanceId))
	{
		SetPreviewHighlight(*OldSelected, false);
	}

	for (const TPair<FName, AActor*>& Pair : PreviewActorMap)
	{
		if (Pair.Value == PreviewActor)
		{
			SelectedInstanceId = Pair.Key;
			SetPreviewHighlight(PreviewActor, true);

			if (LevelEditorWidget)
			{
				LevelEditorWidget->RefreshDetailsPanel();
			}

			UE_LOG(LogTemp, Warning, TEXT("Selected preview actor: %s"), *SelectedInstanceId.ToString());
			return true;
		}
	}

	return false;
}

void ATT_RuntimeLevelEditor::ClearSelection()
{
	if (AActor** OldSelected = PreviewActorMap.Find(SelectedInstanceId))
	{
		SetPreviewHighlight(*OldSelected, false);
	}

	SelectedInstanceId = NAME_None;

	if (LevelEditorWidget)
	{
		LevelEditorWidget->RefreshDetailsPanel();
	}
}

FTT_CustomPlacedActor* ATT_RuntimeLevelEditor::FindSelectedRecord()
{
	for (FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.InstanceId == SelectedInstanceId)
		{
			return &Record;
		}
	}
	return nullptr;
}

const FTT_CustomPlacedActor* ATT_RuntimeLevelEditor::FindSelectedRecordConst() const
{
	for (const FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.InstanceId == SelectedInstanceId)
		{
			return &Record;
		}
	}
	return nullptr;
}

const FTT_CustomPlacedActor* ATT_RuntimeLevelEditor::GetSelectedRecord() const
{
	return FindSelectedRecordConst();
}

FString ATT_RuntimeLevelEditor::GetSelectedTypeAsString() const
{
	const FTT_CustomPlacedActor* Record = FindSelectedRecordConst();
	if (!Record)
	{
		return TEXT("None");
	}

	return StaticEnum<ETT_CustomPlaceableType>()->GetNameStringByValue((int64)Record->Type);
}

TArray<FTT_CustomPlacedActor> ATT_RuntimeLevelEditor::GetPatrolPointRecords() const
{
	TArray<FTT_CustomPlacedActor> Result;
	for (const FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.Type == ETT_CustomPlaceableType::PatrolPoint)
		{
			Result.Add(Record);
		}
	}
	return Result;
}

bool ATT_RuntimeLevelEditor::DeleteSelected()
{
	if (SelectedInstanceId.IsNone())
	{
		return false;
	}

	if (AActor** FoundActor = PreviewActorMap.Find(SelectedInstanceId))
	{
		if (IsValid(*FoundActor))
		{
			(*FoundActor)->Destroy();
		}
		PreviewActorMap.Remove(SelectedInstanceId);
	}

	const FName DeletedId = SelectedInstanceId;

	WorkingActors.RemoveAll([DeletedId](const FTT_CustomPlacedActor& Record)
		{
			return Record.InstanceId == DeletedId;
		});

	// Remove this patrol point from tank links if needed
	for (FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.Type == ETT_CustomPlaceableType::EnemyTank)
		{
			Record.LinkedPatrolPointIds.Remove(DeletedId);
		}
	}

	if (DeletedId == PatrolLinkTankInstanceId)
	{
		PatrolLinkTankInstanceId = NAME_None;
	}

	SelectedInstanceId = NAME_None;

	if (LevelEditorWidget)
	{
		LevelEditorWidget->RefreshDetailsPanel();
	}

	return true;
}

bool ATT_RuntimeLevelEditor::UpdateSelectedTransform(const FTransform& NewTransform)
{
	FTT_CustomPlacedActor* Record = FindSelectedRecord();
	if (!Record)
	{
		return false;
	}

	FTransform AdjustedTransform = NewTransform;
	FVector AdjustedLocation = SnapLocationToGrid(NewTransform.GetLocation());
	AdjustedTransform.SetLocation(AdjustedLocation);

	FString Error;
	if (!IsPlacementLocationValid(Record->Type, AdjustedLocation, Error, Record->InstanceId))
	{
		UE_LOG(LogTemp, Warning, TEXT("Transform update failed: %s"), *Error);
		return false;
	}

	Record->Transform = AdjustedTransform;

	if (AActor** FoundPreview = PreviewActorMap.Find(Record->InstanceId))
	{
		if (IsValid(*FoundPreview))
		{
			(*FoundPreview)->SetActorTransform(AdjustedTransform);
		}
	}

	return true;
}

bool ATT_RuntimeLevelEditor::UpdateSelectedTowerTuning(const FTT_TowerTuning& NewTuning)
{
	FTT_CustomPlacedActor* Record = FindSelectedRecord();
	if (!Record || Record->Type != ETT_CustomPlaceableType::EnemyTower)
	{
		return false;
	}

	Record->TowerTuning = NewTuning;
	return true;
}

bool ATT_RuntimeLevelEditor::UpdateSelectedTankTuning(const FTT_TankTuning& NewTuning)
{
	FTT_CustomPlacedActor* Record = FindSelectedRecord();
	if (!Record || Record->Type != ETT_CustomPlaceableType::EnemyTank)
	{
		return false;
	}

	Record->TankTuning = NewTuning;
	return true;
}

bool ATT_RuntimeLevelEditor::LinkSelectedTankToPatrolPoint(FName PatrolPointId)
{
	FName TargetTankId = PatrolLinkTankInstanceId;

	if (TargetTankId.IsNone())
	{
		const FTT_CustomPlacedActor* Record = FindSelectedRecordConst();
		if (Record && Record->Type == ETT_CustomPlaceableType::EnemyTank)
		{
			TargetTankId = Record->InstanceId;
		}
	}

	if (TargetTankId.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("LinkSelectedTankToPatrolPoint failed: no selected/armed enemy tank."));
		return false;
	}

	return LinkPatrolPointToTank(TargetTankId, PatrolPointId);
}

bool ATT_RuntimeLevelEditor::UnlinkSelectedTankFromPatrolPoint(FName PatrolPointId)
{
	FName TargetTankId = PatrolLinkTankInstanceId;

	if (TargetTankId.IsNone())
	{
		const FTT_CustomPlacedActor* Record = FindSelectedRecordConst();
		if (Record && Record->Type == ETT_CustomPlaceableType::EnemyTank)
		{
			TargetTankId = Record->InstanceId;
		}
	}

	if (TargetTankId.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlinkSelectedTankFromPatrolPoint failed: no selected/armed enemy tank."));
		return false;
	}

	return UnlinkPatrolPointFromTank(TargetTankId, PatrolPointId);
}

bool ATT_RuntimeLevelEditor::ValidateLevel(FString& OutError) const
{
	int32 EnemyCount = 0;
	int32 PlayerStartCount = 0;
	TSet<FName> PatrolIds;

	for (const FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.Type == ETT_CustomPlaceableType::EnemyTower ||
			Record.Type == ETT_CustomPlaceableType::EnemyTank)
		{
			EnemyCount++;
		}

		if (Record.Type == ETT_CustomPlaceableType::PlayerStart)
		{
			PlayerStartCount++;
		}

		if (Record.Type == ETT_CustomPlaceableType::PatrolPoint)
		{
			PatrolIds.Add(Record.InstanceId);
		}
	}

	if (EnemyCount <= 0)
	{
		OutError = TEXT("Level must contain at least one enemy.");
		return false;
	}

	if (PlayerStartCount != 1)
	{
		OutError = TEXT("Level must contain exactly one PlayerStart.");
		return false;
	}

	for (const FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.Type == ETT_CustomPlaceableType::EnemyTank)
		{
			for (const FName& PatrolId : Record.LinkedPatrolPointIds)
			{
				if (!PatrolIds.Contains(PatrolId))
				{
					OutError = FString::Printf(TEXT("Tank references missing patrol point: %s"), *PatrolId.ToString());
					return false;
				}
			}
		}
	}

	return true;
}

bool ATT_RuntimeLevelEditor::SaveLevel(const FString& LevelName, bool bAddToCampaign)
{
	FString Error;
	if (!ValidateLevel(Error))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save failed: %s"), *Error);
		return false;
	}

	UTT_CustomLevelSaveGame* SaveObj = Cast<UTT_CustomLevelSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UTT_CustomLevelSaveGame::StaticClass()));

	if (!SaveObj)
	{
		return false;
	}

	bool bFinalAddToCampaign = bAddToCampaign;

	// Preserve previous "add to campaign" state unless explicitly disabled by deletion or another dedicated action.
	if (!bFinalAddToCampaign && UGameplayStatics::DoesSaveGameExist(GCustomLevelSlotName, GCustomLevelSlotIndex))
	{
		if (UTT_CustomLevelSaveGame* ExistingSave = Cast<UTT_CustomLevelSaveGame>(
			UGameplayStatics::LoadGameFromSlot(GCustomLevelSlotName, GCustomLevelSlotIndex)))
		{
			bFinalAddToCampaign = ExistingSave->SavedLevel.bAddToCampaignProgression;
		}
	}

	SaveObj->SavedLevel.LevelDisplayName = LevelName;
	SaveObj->SavedLevel.PlacedActors = WorkingActors;
	SaveObj->SavedLevel.bAddToCampaignProgression = bFinalAddToCampaign;

	UE_LOG(LogTemp, Warning, TEXT("SaveLevel: LevelName=%s RequestedAdd=%s FinalAdd=%s"),
		*LevelName,
		bAddToCampaign ? TEXT("true") : TEXT("false"),
		bFinalAddToCampaign ? TEXT("true") : TEXT("false"));

	for (const FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.Type == ETT_CustomPlaceableType::EnemyTank)
		{
			UE_LOG(LogTemp, Warning, TEXT("Saving tank %s with %d patrol links"),
				*Record.InstanceId.ToString(),
				Record.LinkedPatrolPointIds.Num());

			for (const FName& PatrolId : Record.LinkedPatrolPointIds)
			{
				UE_LOG(LogTemp, Warning, TEXT("  PatrolLink -> %s"), *PatrolId.ToString());
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("SaveLevel: LevelName=%s AddToCampaign=%s"),
		*LevelName,
		bAddToCampaign ? TEXT("true") : TEXT("false"));

	const bool bSaved = UGameplayStatics::SaveGameToSlot(
		SaveObj,
		GCustomLevelSlotName,
		GCustomLevelSlotIndex);

	if (bSaved)
	{
		if (UTT_GameInstance* GI = Cast<UTT_GameInstance>(GetGameInstance()))
		{
			GI->bHasSavedCustomLevel = true;
			GI->SetCustomLevelAddedToProgression(bFinalAddToCampaign);
		}
	}

	return bSaved;
}

bool ATT_RuntimeLevelEditor::LoadLevel()
{
	if (!UGameplayStatics::DoesSaveGameExist(GCustomLevelSlotName, GCustomLevelSlotIndex))
	{
		return false;
	}

	UTT_CustomLevelSaveGame* SaveObj = Cast<UTT_CustomLevelSaveGame>(
		UGameplayStatics::LoadGameFromSlot(GCustomLevelSlotName, GCustomLevelSlotIndex));

	if (!SaveObj)
	{
		return false;
	}

	WorkingActors = SaveObj->SavedLevel.PlacedActors;
	SelectedInstanceId = NAME_None;
	RebuildAllPreviewActors();

	if (LevelEditorWidget)
	{
		LevelEditorWidget->RefreshDetailsPanel();
	}

	return true;
}

void ATT_RuntimeLevelEditor::PlayNow()
{
	if (UTT_GameInstance* GI = Cast<UTT_GameInstance>(GetGameInstance()))
	{
		GI->PlayCustomLevelNow();
	}
}

void ATT_RuntimeLevelEditor::ClearCurrentLevel()
{
	for (TPair<FName, AActor*>& Pair : PreviewActorMap)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->Destroy();
		}
	}
	PreviewActorMap.Empty();

	WorkingActors.Empty();
	SelectedInstanceId = NAME_None;

	bGhostPlacementValid = false;
	RefreshGhostPreview();

	UE_LOG(LogTemp, Log, TEXT("Level Editor: Cleared current level contents."));
}

bool ATT_RuntimeLevelEditor::DeleteCurrentLevel()
{
	ClearCurrentLevel();

	bool bDeletedSave = true;
	if (UGameplayStatics::DoesSaveGameExist(GCustomLevelSlotName, GCustomLevelSlotIndex))
	{
		bDeletedSave = UGameplayStatics::DeleteGameInSlot(GCustomLevelSlotName, GCustomLevelSlotIndex);
		if (!bDeletedSave)
		{
			UE_LOG(LogTemp, Warning, TEXT("Level Editor: Failed to delete custom level save slot."));
			return false;
		}
	}

	if (UTT_GameInstance* GI = Cast<UTT_GameInstance>(GetGameInstance()))
	{
		GI->bHasSavedCustomLevel = false;
		GI->SetCustomLevelAddedToProgression(false);
	}

	UE_LOG(LogTemp, Log, TEXT("Level Editor: Deleted current custom level and removed it from campaign."));
	return true;
}

bool ATT_RuntimeLevelEditor::BeginPatrolLinkForSelectedTank()
{
	const FTT_CustomPlacedActor* Record = FindSelectedRecordConst();
	if (!Record || Record->Type != ETT_CustomPlaceableType::EnemyTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginPatrolLinkForSelectedTank failed: selected actor is not an enemy tank."));
		return false;
	}

	PatrolLinkTankInstanceId = Record->InstanceId;

	UE_LOG(LogTemp, Warning, TEXT("Patrol linking armed for tank: %s"), *PatrolLinkTankInstanceId.ToString());
	return true;
}

void ATT_RuntimeLevelEditor::ClearPatrolLinkTarget()
{
	PatrolLinkTankInstanceId = NAME_None;
}

bool ATT_RuntimeLevelEditor::LinkPatrolPointToTank(FName TankInstanceId, FName PatrolPointId)
{
	if (TankInstanceId.IsNone() || PatrolPointId.IsNone())
	{
		return false;
	}

	FTT_CustomPlacedActor* TankRecord = nullptr;
	bool bPatrolExists = false;

	for (FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.InstanceId == TankInstanceId &&
			Record.Type == ETT_CustomPlaceableType::EnemyTank)
		{
			TankRecord = &Record;
		}

		if (Record.InstanceId == PatrolPointId &&
			Record.Type == ETT_CustomPlaceableType::PatrolPoint)
		{
			bPatrolExists = true;
		}
	}

	if (!TankRecord)
	{
		UE_LOG(LogTemp, Warning, TEXT("LinkPatrolPointToTank failed: tank record not found: %s"), *TankInstanceId.ToString());
		return false;
	}

	if (!bPatrolExists)
	{
		UE_LOG(LogTemp, Warning, TEXT("LinkPatrolPointToTank failed: patrol point record not found: %s"), *PatrolPointId.ToString());
		return false;
	}

	if (!TankRecord->LinkedPatrolPointIds.Contains(PatrolPointId))
	{
		TankRecord->LinkedPatrolPointIds.Add(PatrolPointId);
		UE_LOG(LogTemp, Warning, TEXT("Linked patrol point %s to tank %s"),
			*PatrolPointId.ToString(), *TankInstanceId.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("Tank %s now has %d patrol links"),
		*TankInstanceId.ToString(),
		TankRecord->LinkedPatrolPointIds.Num());

	for (const FName& Id : TankRecord->LinkedPatrolPointIds)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Linked -> %s"), *Id.ToString());
	}

	return true;
}

bool ATT_RuntimeLevelEditor::UnlinkPatrolPointFromTank(FName TankInstanceId, FName PatrolPointId)
{
	if (TankInstanceId.IsNone() || PatrolPointId.IsNone())
	{
		return false;
	}

	for (FTT_CustomPlacedActor& Record : WorkingActors)
	{
		if (Record.InstanceId == TankInstanceId &&
			Record.Type == ETT_CustomPlaceableType::EnemyTank)
		{
			Record.LinkedPatrolPointIds.Remove(PatrolPointId);
			UE_LOG(LogTemp, Warning, TEXT("Unlinked patrol point %s from tank %s"), *PatrolPointId.ToString(), *TankInstanceId.ToString());
			return true;
		}
	}

	return false;
}