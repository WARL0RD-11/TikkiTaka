// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TT_PlayerController.generated.h"

/**
 * 
 */
class UEnhancedInputComponent;
class UInputMappingContext;
class UEnhancedInputSubsystems;
class UInputAction;
struct FInputActionValue;
class ATT_TankPawn;	
class ICommand;

UENUM()
enum class EInputAction : uint8
{
	None,
	Move,
	Aim,
	Fire,
};

UCLASS()
class BATTLEBLASTER_API ATT_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;



	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> FireAction;

	ATT_TankPawn* PlayerPawnTank;

private:
	TMap<EInputAction, TSharedPtr<ICommand>> CommandMap;

	UPROPERTY(EditAnywhere, Category = "Movement|Speed")
	float MoveSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Movement|Turn")
	float TurnSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Movement|Aim")
	float AimSpeed = 100.f;

private:
	void HandleMove(const FInputActionValue& ActionValue);
	void HandleAim(const FInputActionValue& ActionValue);
	void HandleFire(const FInputActionValue& ActionValue);
	
};
