// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "TT_LevelEditorController.generated.h"

/**
 * 
 */
class ATT_RuntimeLevelEditorManager;
class UInputMappingContext;
class UInputAction;

class ATT_RuntimeLevelEditor;

UCLASS()
class BATTLEBLASTER_API ATT_LevelEditorController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LeftClickAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RotateLeftAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RotateRightAction;

	UPROPERTY(BlueprintReadOnly, Category = "Editor")
	float PlacementYawDegrees = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Editor")
	TObjectPtr<ATT_RuntimeLevelEditor> EditorManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
	bool bEditorInputDisabled = false;

protected:
	UFUNCTION()
	void HandleLeftClick(const FInputActionValue& ActionValue);

	UFUNCTION()
	void RotatePlacementLeft(const FInputActionValue& ActionValue);

	UFUNCTION()
	void RotatePlacementRight(const FInputActionValue& ActionValue);

	bool GetCursorHit(FHitResult& OutHit) const;
	void FindEditorManager();
	void SetEditorInputDisabled(bool bDisabled);
	
};
