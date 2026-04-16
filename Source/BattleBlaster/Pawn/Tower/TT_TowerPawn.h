// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "TT_TowerPawn.generated.h"

/**
 * 
 */
class UTT_TowerAIComponent;

UCLASS()
class BATTLEBLASTER_API ATT_TowerPawn : public ATT_BasePawn
{
	GENERATED_BODY()
public:
	ATT_TowerPawn();
	virtual void HandleDestruction() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UTT_TowerAIComponent> TowerAIComponent;

private:
	bool bRegisteredWithGameMode = false;

	class ATikkiTakaGameMode* GM = nullptr;

	
};
