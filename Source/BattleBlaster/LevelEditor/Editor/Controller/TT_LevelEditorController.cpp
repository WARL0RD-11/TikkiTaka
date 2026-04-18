// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_LevelEditorController.h"

#include "../TT_RuntimeLevelEditor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "EngineUtils.h"
#include "Engine/World.h"

void ATT_LevelEditorController::BeginPlay()
{
	Super::BeginPlay();

	FindEditorManager();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			if (MappingContext)
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}

	SetShowMouseCursor(true);
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	PrimaryActorTick.bCanEverTick = true;
}

void ATT_LevelEditorController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bEditorInputDisabled)
	{
		return;
	}

	if (!EditorManager)
	{
		return;
	}

	FHitResult Hit;
	if (!GetCursorHit(Hit))
	{
		return;
	}

	const FRotator PlacementRot(0.f, PlacementYawDegrees, 0.f);
	EditorManager->UpdateGhostFromWorldLocation(Hit.ImpactPoint, PlacementRot);
}

void ATT_LevelEditorController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (LeftClickAction)
		{
			EIC->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ATT_LevelEditorController::HandleLeftClick);
		}
		if (RotateLeftAction)
		{
			EIC->BindAction(RotateLeftAction, ETriggerEvent::Started, this, &ATT_LevelEditorController::RotatePlacementLeft);
		}

		if (RotateRightAction)
		{
			EIC->BindAction(RotateRightAction, ETriggerEvent::Started, this, &ATT_LevelEditorController::RotatePlacementRight);
		}
	}
}

void ATT_LevelEditorController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			if (MappingContext)
			{
				Subsystem->RemoveMappingContext(MappingContext);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ATT_LevelEditorController::HandleLeftClick(const FInputActionValue& ActionValue)
{
	if (bEditorInputDisabled)
	{
		return;
	}

	if (!EditorManager)
	{
		FindEditorManager();
		if (!EditorManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("LevelEditorPC: No RuntimeLevelEditor found."));
			return;
		}
	}

	FHitResult Hit;
	if (!GetCursorHit(Hit))
	{
		return;
	}

	if (AActor* HitActor = Hit.GetActor())
	{
		if (EditorManager->SelectByPreviewActor(HitActor))
		{
			return;
		}
	}

	if (!EditorManager->IsGhostPlacementValid())
	{
		return;
	}

	const FRotator PlacementRot(0.f, PlacementYawDegrees, 0.f);
	EditorManager->PlaceAtWorldLocation(Hit.ImpactPoint, PlacementRot);
}

void ATT_LevelEditorController::RotatePlacementLeft(const FInputActionValue& ActionValue)
{
	PlacementYawDegrees -= 90.f;
}

void ATT_LevelEditorController::RotatePlacementRight(const FInputActionValue& ActionValue)
{
	PlacementYawDegrees += 90.f;
}


bool ATT_LevelEditorController::GetCursorHit(FHitResult& OutHit) const
{
	return GetHitResultUnderCursor(ECC_Visibility, false, OutHit);
}

void ATT_LevelEditorController::FindEditorManager()
{
	EditorManager = nullptr;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ATT_RuntimeLevelEditor> It(World); It; ++It)
	{
		EditorManager = *It;
		break;
	}
}

void ATT_LevelEditorController::SetEditorInputDisabled(bool bDisabled)
{
	bEditorInputDisabled = bDisabled;
}
