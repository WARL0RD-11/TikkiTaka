// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/LocalPlayer.h"
#include "BattleBlaster/Commands/Move/TT_MoveCommand.h"
#include "BattleBlaster/Commands/Aim/TT_AimCommand.h"
#include "BattleBlaster/Commands/Fire/TT_FireCommand.h"
#include "BattleBlaster/Commands/Pause/TT_PauseCommand.h"
#include "BattleBlaster/Pawn/Tank/TT_TankPawn.h"

void ATT_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawnTank = Cast<ATT_TankPawn>(GetPawn());
	if (!PlayerPawnTank) return;

	DefaultKeyMap.Add(MoveForwardAction, EKeys::W);
	DefaultKeyMap.Add(MoveBackwardAction, EKeys::S);
	DefaultKeyMap.Add(TurnLeftAction, EKeys::A);
	DefaultKeyMap.Add(TurnRightAction, EKeys::D);
	DefaultKeyMap.Add(FireAction, EKeys::LeftMouseButton);
	DefaultKeyMap.Add(PauseAction, EKeys::P);


	CommandMap.Add(EInputAction::Move, MakeShareable( new TT_MoveCommand()));	
	CommandMap.Add(EInputAction::Aim, MakeShareable(new TT_AimCommand()));	
	CommandMap.Add(EInputAction::Fire, MakeShareable(new TT_FireCommand()));	
	CommandMap.Add(EInputAction::Pause, MakeShareable(new TT_PauseCommand(this, PauseMenuWidgetClass)));

	bInputDisabled = true;

	auto* USubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	USubsystem->AddMappingContext(MappingContext, 0);

	SetShowMouseCursor(true);
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	PrimaryActorTick.bCanEverTick = true;
}

void ATT_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bInputDisabled)
	{
		return;
	}

	UpdateAimUnderCursor();
}

void ATT_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		//EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATT_PlayerController::HandleMove);
		EIC->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ThisClass::OnMoveForwardAction);
		EIC->BindAction(MoveBackwardAction, ETriggerEvent::Triggered, this, &ThisClass::OnMoveBackwardAction);
		EIC->BindAction(TurnLeftAction, ETriggerEvent::Triggered, this, &ThisClass::OnTurnLeftAction);
		EIC->BindAction(TurnRightAction, ETriggerEvent::Triggered, this, &ThisClass::OnTurnRightAction);
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::HandleFire);
		EIC->BindAction(PauseAction, ETriggerEvent::Started, this, &ThisClass::HandlePause);
	}
}

void ATT_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void ATT_PlayerController::HandleAim(const FInputActionValue& ActionValue)
{
	if (bInputDisabled)
	{
		return;
	}

	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (PlayerPawnTank && CommandMap.Contains(EInputAction::Aim))
	{
		CommandMap[EInputAction::Aim]->Execute(Cast<ATT_BasePawn>(PlayerPawnTank), ActionValue, HitResult.ImpactPoint, AimSpeed);
	}
}

void ATT_PlayerController::HandleFire(const FInputActionValue& ActionValue)
{
	if (bInputDisabled)
	{
		return;
	}

	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (PlayerPawnTank && CommandMap.Contains(EInputAction::Fire))
	{
		CommandMap[EInputAction::Fire]->Execute(Cast<ATT_BasePawn>(PlayerPawnTank), ActionValue, HitResult.ImpactPoint, AimSpeed);
	}
}

void ATT_PlayerController::HandlePause(const FInputActionValue& ActionValue)
{
	if (!CommandMap.Contains(EInputAction::Pause))
	{
		return;
	}

	const TSharedPtr<ICommand>* FoundCommand = CommandMap.Find(EInputAction::Pause);
	if (!FoundCommand || !FoundCommand->IsValid())
	{
		return;
	}

	const FInputActionValue DummyValue;
	FoundCommand->Get()->Execute(nullptr, DummyValue, 0.f, 0.f);
}

void ATT_PlayerController::UpdateAimUnderCursor()
{
	if (!PlayerPawnTank)
	{
		return;
	}

	FHitResult HitResult;
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (!bHit)
	{
		return;
	}

	if (CommandMap.Contains(EInputAction::Aim))
	{
		const FInputActionValue DummyValue;
		CommandMap[EInputAction::Aim]->Execute(
			Cast<ATT_BasePawn>(PlayerPawnTank),
			DummyValue,
			HitResult.ImpactPoint,
			AimSpeed
		);
	}
}

void ATT_PlayerController::OnMoveForwardAction(const FInputActionValue& ActionValue)
{
	if (bInputDisabled || !PlayerPawnTank)
	{
		return;
	}

	const TSharedPtr<ICommand>* FoundCommand = CommandMap.Find(EInputAction::Move);
	if (!FoundCommand || !FoundCommand->IsValid())
	{
		return;
	}

	TT_MoveCommand* MoveCommand = static_cast<TT_MoveCommand*>(FoundCommand->Get());
	if (!MoveCommand)
	{
		return;
	}

	const float InputValue = ActionValue.Get<float>();
	const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;

	MoveCommand->ExecuteForward(
		Cast<ATT_BasePawn>(PlayerPawnTank),
		InputValue,
		DeltaTime,
		MoveSpeed
	);
}

void ATT_PlayerController::OnMoveBackwardAction(const FInputActionValue& ActionValue)
{
	if (bInputDisabled || !PlayerPawnTank)
	{
		return;
	}

	const TSharedPtr<ICommand>* FoundCommand = CommandMap.Find(EInputAction::Move);
	if (!FoundCommand || !FoundCommand->IsValid())
	{
		return;
	}

	TT_MoveCommand* MoveCommand = static_cast<TT_MoveCommand*>(FoundCommand->Get());
	if (!MoveCommand)
	{
		return;
	}

	const float InputValue = -ActionValue.Get<float>();
	const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;

	MoveCommand->ExecuteForward(
		Cast<ATT_BasePawn>(PlayerPawnTank),
		InputValue,
		DeltaTime,
		MoveSpeed
	);
}

void ATT_PlayerController::OnTurnRightAction(const FInputActionValue& ActionValue)
{
	if (bInputDisabled || !PlayerPawnTank)
	{
		return;
	}

	const TSharedPtr<ICommand>* FoundCommand = CommandMap.Find(EInputAction::Move);
	if (!FoundCommand || !FoundCommand->IsValid())
	{
		return;
	}

	TT_MoveCommand* MoveCommand = static_cast<TT_MoveCommand*>(FoundCommand->Get());
	if (!MoveCommand)
	{
		return;
	}

	const float InputValue = ActionValue.Get<float>();
	const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;

	MoveCommand->ExecuteTurn(
		Cast<ATT_BasePawn>(PlayerPawnTank),
		InputValue,
		DeltaTime,
		TurnSpeed
	);
}

void ATT_PlayerController::OnTurnLeftAction(const FInputActionValue& ActionValue)
{
	if (bInputDisabled || !PlayerPawnTank)
	{
		return;
	}

	const TSharedPtr<ICommand>* FoundCommand = CommandMap.Find(EInputAction::Move);
	if (!FoundCommand || !FoundCommand->IsValid())
	{
		return;
	}

	TT_MoveCommand* MoveCommand = static_cast<TT_MoveCommand*>(FoundCommand->Get());
	if (!MoveCommand)
	{
		return;
	}

	const float InputValue = -ActionValue.Get<float>();
	const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;

	MoveCommand->ExecuteTurn(
		Cast<ATT_BasePawn>(PlayerPawnTank),
		InputValue,
		DeltaTime,
		TurnSpeed
	);
}

void ATT_PlayerController::SetPlayerInputDisabled(bool bDisabled)
{
	bInputDisabled = bDisabled;
}

void ATT_PlayerController::RebindKey(UInputAction* Action, FKey NewKey)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!Subsystem || !MappingContext || !Action || !NewKey.IsValid())
	{
		return;
	}

	if (!DefaultKeyMap.Contains(Action))
	{
		return;
	}

	Subsystem->RemoveMappingContext(MappingContext);

	const TArray<FEnhancedActionKeyMapping> Mappings = MappingContext->GetMappings();
	for (int32 i = Mappings.Num() - 1; i >= 0; --i)
	{
		if (Mappings[i].Action == Action)
		{
			MappingContext->UnmapKey(Action, Mappings[i].Key);
		}
	}

	MappingContext->MapKey(Action, NewKey);

	Subsystem->AddMappingContext(MappingContext, 0);
}

void ATT_PlayerController::ResetToDefaultKeys()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!Subsystem || !MappingContext)
	{
		return;
	}

	Subsystem->RemoveMappingContext(MappingContext);

	for (const TPair<TObjectPtr<UInputAction>, FKey>& Pair : DefaultKeyMap)
	{
		UInputAction* Action = Pair.Key.Get();
		const FKey DefaultKey = Pair.Value;

		if (!Action || !DefaultKey.IsValid())
		{
			continue;
		}

		const TArray<FEnhancedActionKeyMapping> CurrentMappings = MappingContext->GetMappings();
		for (int32 i = CurrentMappings.Num() - 1; i >= 0; --i)
		{
			if (CurrentMappings[i].Action == Action)
			{
				MappingContext->UnmapKey(Action, CurrentMappings[i].Key);
			}
		}

		MappingContext->MapKey(Action, DefaultKey);
	}

	Subsystem->AddMappingContext(MappingContext, 0);

}

FKey ATT_PlayerController::GetCurrentKeyForAction(UInputAction* Action) const
{
	if (!Action)
	{
		return FKey();
	}

	if (MappingContext)
	{
		const TArray<FEnhancedActionKeyMapping> Mappings = MappingContext->GetMappings();
		for (const FEnhancedActionKeyMapping& Mapping : Mappings)
		{
			if (Mapping.Action == Action)
			{
				return Mapping.Key;
			}
		}
	}

	if (const FKey* DefaultKey = DefaultKeyMap.Find(Action))
	{
		return *DefaultKey;
	}

	return FKey();
}

void ATT_PlayerController::ApplyGameplayInputMode()
{
	SetShowMouseCursor(true);
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}
