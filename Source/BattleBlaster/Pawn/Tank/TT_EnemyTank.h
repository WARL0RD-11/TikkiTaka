// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "TT_EnemyTank.generated.h"

/**
 * 
 */

class UTT_TankAIComponent;

UCLASS()
class BATTLEBLASTER_API ATT_EnemyTank : public ATT_BasePawn
{
	GENERATED_BODY()
	
public:
	ATT_EnemyTank();
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void HandleDestruction() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UTT_TankAIComponent> TankAIComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnSpeed = 2.5f;

protected:
	virtual void BeginPlay() override;

private:
	bool bRegisteredWithGameMode = false;

	class ATikkiTakaGameMode* GM = nullptr;

};
