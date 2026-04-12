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
#include "BattleBlaster/Pawn/Tank/TT_TankPawn.h"

void ATT_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawnTank = Cast<ATT_TankPawn>(GetPawn());
	if (!PlayerPawnTank) return;

	CommandMap.Add(EInputAction::Move, MakeShared<TT_MoveCommand>());	
	CommandMap.Add(EInputAction::Aim, MakeShared<TT_AimCommand>());	
	CommandMap.Add(EInputAction::Fire, MakeShared<TT_FireCommand>());	

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
	UpdateAimUnderCursor();

	//Print Command Map for debugging
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Command Map: %d"), CommandMap.Num()));
}

void ATT_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATT_PlayerController::HandleMove);
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ATT_PlayerController::HandleFire);
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

void ATT_PlayerController::HandleMove(const FInputActionValue& ActionValue)
{

	if(PlayerPawnTank && CommandMap.Contains(EInputAction::Move))
	{
		CommandMap[EInputAction::Move]->Execute(Cast<ATT_BasePawn>(PlayerPawnTank), ActionValue, MoveSpeed, TurnSpeed);
	}
}

void ATT_PlayerController::HandleAim(const FInputActionValue& ActionValue)
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (PlayerPawnTank && CommandMap.Contains(EInputAction::Aim))
	{
		CommandMap[EInputAction::Aim]->Execute(Cast<ATT_BasePawn>(PlayerPawnTank), ActionValue, HitResult.ImpactPoint, AimSpeed);
	}
}

void ATT_PlayerController::HandleFire(const FInputActionValue& ActionValue)
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (PlayerPawnTank && CommandMap.Contains(EInputAction::Fire))
	{
		CommandMap[EInputAction::Fire]->Execute(Cast<ATT_BasePawn>(PlayerPawnTank), ActionValue, HitResult.ImpactPoint, AimSpeed);
	}
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
