// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

void ATT_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto* USubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	USubsystem->AddMappingContext(MappingContext, 0);
}

void ATT_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATT_PlayerController::HandleMove);
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

void ATT_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);	
}

void ATT_PlayerController::HandleMove(const FInputActionValue& ActionValue)
{
}
