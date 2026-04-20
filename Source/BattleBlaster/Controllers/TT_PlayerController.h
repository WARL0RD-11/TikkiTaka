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
class UUserWidget;	

UENUM()
enum class EInputAction : uint8
{
	None,
	Move,
	Aim,
	Fire,
	Pause,
};

UCLASS()
class BATTLEBLASTER_API ATT_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetPlayerInputDisabled(bool bDisabled);
	bool IsPlayerInputDisabled() const { return bInputDisabled; }

	UFUNCTION(BlueprintCallable)
	void RebindKey(UInputAction* Action, FKey NewKey);

	UFUNCTION(BlueprintCallable)
	void ResetToDefaultKeys();

	UFUNCTION(BlueprintCallable)
	FKey GetCurrentKeyForAction(UInputAction* Action) const;

	UFUNCTION(BlueprintCallable)
	void ApplyGameplayInputMode();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;



	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveBackwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> TurnRightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> TurnLeftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	ATT_TankPawn* PlayerPawnTank;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bInputDisabled = false;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenuWidget = nullptr;

private:
	TMap<EInputAction, TSharedPtr<ICommand>> CommandMap;

	UPROPERTY()
	TMap<UInputAction*, FKey> DefaultKeyMap;

	UPROPERTY(EditAnywhere, Category = "Movement|Speed")
	float MoveSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Movement|Turn")
	float TurnSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Movement|Aim")
	float AimSpeed = 100.f;

	void HandleAim(const FInputActionValue& ActionValue);
	void HandleFire(const FInputActionValue& ActionValue);
	void HandlePause(const FInputActionValue& ActionValue);
	void UpdateAimUnderCursor();

	//-------------------------------------------------------
	void OnMoveForwardAction(const FInputActionValue& ActionValue);
	void OnMoveBackwardAction(const FInputActionValue& ActionValue);
	void OnTurnRightAction(const FInputActionValue& ActionValue);
	void OnTurnLeftAction(const FInputActionValue& ActionValue);
	
};
