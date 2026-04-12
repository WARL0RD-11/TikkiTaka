// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "TT_TowerPawn.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API ATT_TowerPawn : public ATT_BasePawn
{
	GENERATED_BODY()
public:
	virtual void HandleDestruction() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool bRegisteredWithGameMode = false;

	
};
