// TT_TowerAIComponent.h
#pragma once

#include "CoreMinimal.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPBrainComponent.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPAction.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPPlanner.h"
#include "TT_TowerAIComponent.generated.h"

class ATT_BasePawn;

namespace TTTowerGOAPKeys
{
    extern const FName HasTarget;
    extern const FName TargetInRange;
    extern const FName HasLineOfSight;
    extern const FName TurretAligned;
    extern const FName CanFire;
    extern const FName AttackComplete;
}

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class BATTLEBLASTER_API UTT_TowerAIComponent : public UTT_GOAPBrainComponent
{
    GENERATED_BODY()

public:
    UTT_TowerAIComponent();

    virtual void BeginPlay() override;

    virtual void BuildActions() override;
    virtual TMap<FName, bool> BuildWorldState() const override;
    virtual TMap<FName, bool> BuildGoalState() const override;
    virtual bool ShouldReplan() const override;
    bool ScanForPlayer();
    bool HasValidTarget() const;
    bool IsTargetInRange() const;
    bool HasLineOfSightToTarget() const;
    bool IsTurretAligned() const;
    bool CanFire() const;
    void AimAtTarget();
    bool FireAtTarget();

protected:
    FVector GetAimPoint() const;

    UPROPERTY(EditAnywhere, Category = "Tower|Config")
    float ScanRange = 3000.f;

    UPROPERTY(EditAnywhere, Category = "Tower|Config")
    float FireRange = 2000.f;

    UPROPERTY(EditAnywhere, Category = "Tower|Config")
    float AimToleranceDegrees = 3.f;

    UPROPERTY(EditAnywhere, Category = "Tower|Config")
    float AimSpeed = 5.f;

    UPROPERTY(EditAnywhere, Category = "Tower|Config")
    float FireCooldown = 1.f;

private:
    UPROPERTY()
    APawn* CurrentTarget = nullptr;

    UPROPERTY()
    ATT_BasePawn* mOwnerPawn = nullptr;

    float LastFireTime = -1e9f;

    mutable bool bWasAlignedLastFrame = false;
};


UCLASS()
class BATTLEBLASTER_API UTT_TowerAction_Scan : public UTT_GOAPAction
{
    GENERATED_BODY()
public:
    UTT_TowerAction_Scan();
    virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override;
    virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override;
};

UCLASS()
class BATTLEBLASTER_API UTT_TowerAction_CheckTarget : public UTT_GOAPAction
{
    GENERATED_BODY()
public:
    UTT_TowerAction_CheckTarget();
    virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override;
    virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override;
};

UCLASS()
class BATTLEBLASTER_API UTT_TowerAction_Aim : public UTT_GOAPAction
{
    GENERATED_BODY()
public:
    UTT_TowerAction_Aim();
    virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override;
    virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override;
};

UCLASS()
class BATTLEBLASTER_API UTT_TowerAction_Fire : public UTT_GOAPAction
{
    GENERATED_BODY()
public:
    UTT_TowerAction_Fire();
    virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override;
    virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override;
};