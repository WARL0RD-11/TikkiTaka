#pragma once

#include "CoreMinimal.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPBrainComponent.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPAction.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPPlanner.h"
#include "TT_TankAIComponent.generated.h"

class APawn;
class ATT_HealthPickup;
class ATT_EnemyTank;

namespace TTTankGOAPKeys
{
	static const FName HasTarget(TEXT("HasTarget"));
	static const FName TargetInScanRange(TEXT("TargetInScanRange"));
	static const FName TargetInAttackRange(TEXT("TargetInAttackRange"));
	static const FName LowHealth(TEXT("LowHealth"));
	static const FName HasHealthPickup(TEXT("HasHealthPickup"));
	static const FName ReachedHealthPickup(TEXT("ReachedHealthPickup"));
	static const FName HasPatrolPoint(TEXT("HasPatrolPoint"));
	static const FName ReachedPatrolPoint(TEXT("ReachedPatrolPoint"));
	static const FName CanFire(TEXT("CanFire"));
	static const FName TurretAligned(TEXT("TurretAligned"));
	static const FName WantsAttack(TEXT("WantsAttack"));
	static const FName WantsRecovery(TEXT("WantsRecovery"));
}

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class BATTLEBLASTER_API UTT_TankAIComponent : public UTT_GOAPBrainComponent
{
	GENERATED_BODY()

public:
	UTT_TankAIComponent();

public:
	UPROPERTY()
	ATT_BasePawn* mOwnerPawn = nullptr;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float ScanRange = 1800.f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float ChaseRange = 1400.f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float AttackRange = 950.f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float PatrolPointAcceptanceRadius = 120.f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float PickupAcceptanceRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float FireCooldown = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float AimSpeed = 5.f;

	UPROPERTY()
	bool bCommittedToRecovery = false;

	UPROPERTY()
	bool bCommittedToAttack = false;

	UPROPERTY(EditAnywhere, Category = "AI|Combat")
	float FireAngleTolerance = 10.f;

	UPROPERTY(EditAnywhere, Category = "AI|Combat")
	float FireHoldTime = 0.15f;

	UPROPERTY(EditAnywhere, Category = "AI|Combat")
	float LostTargetGraceTime = 1.0f;

	float TurretAlignedTime = 0.f;
	float LastSeenTargetTime = -1000.f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float LowHealthThreshold = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	float RecoverHealthThreshold = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Tank AI")
	TArray<AActor*> PatrolPoints;

	UPROPERTY()
	TObjectPtr<APawn> CurrentTarget;

	UPROPERTY()
	TObjectPtr<ATT_HealthPickup> TargetHealthPickup;

	UPROPERTY()
	AActor* CurrentPatrolPoint = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI|Debug")
	bool bDebugAI = true;

	UPROPERTY(EditAnywhere, Category = "AI|Debug")
	bool bDebugAISpamLog = false;

	UFUNCTION(BlueprintCallable, Category = "AI|Debug")
	FString GetCurrentDebugStateString() const;

	UFUNCTION(BlueprintCallable, Category = "AI|Debug")
	FString GetCurrentGoalString() const;

	UFUNCTION(BlueprintCallable, Category = "AI|Debug")
	void DebugDrawAIState() const;

	UPROPERTY()
	int32 PatrolIndex = -1;

	double LastFireTime = -1000.0;

	virtual void BuildActions() override;
	virtual TMap<FName, bool> BuildWorldState() const override;
	virtual TMap<FName, bool> BuildGoalState() const override;
	virtual bool ShouldReplan() const override;

	bool HasValidTarget() const;
	bool IsTargetInScanRange() const;
	bool IsTargetInAttackRange() const;
	bool CanFire() const;
	bool IsLowHealth() const;
	bool IsRecoveredEnough() const;
	bool HasHealthPickupTarget() const;
	bool ReachedHealthPickup() const;
	bool HasPatrolTarget() const;
	bool ReachedPatrolPoint() const;
	bool IsTurretAligned() const;

	bool ScanForPlayer();
	bool AcquireHealthPickup();
	bool ChoosePatrolPoint();

	void MoveToTarget(float DeltaTime);
	void MoveToHealthPickup(float DeltaTime);
	void MoveToPatrolPoint(float DeltaTime);
	void AimAtTarget();
	void FireAtTarget();
	void TryFireReactive();

	void UpdateCommitmentFlags();
	void ClearInvalidTargets();

	FVector GetAimPoint() const;
	ATT_EnemyTank* GetTankOwner() const;
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_FindPatrolPoint : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_FindPatrolPoint()
	{
		ActionName = TEXT("FindPatrolPoint");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, false);
		Effects.Add(TTTankGOAPKeys::HasPatrolPoint, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			return TankAI->ChoosePatrolPoint();
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		const UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain);
		return TankAI && TankAI->HasPatrolTarget();
	}
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_MoveToPatrol : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_MoveToPatrol()
	{
		ActionName = TEXT("MoveToPatrol");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, false);
		Preconditions.Add(TTTankGOAPKeys::HasPatrolPoint, true);
		Effects.Add(TTTankGOAPKeys::ReachedPatrolPoint, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			TankAI->MoveToPatrolPoint(DeltaTime);
			return true;
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		const UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain);
		return TankAI && TankAI->ReachedPatrolPoint();
	}
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_ScanPlayer : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_ScanPlayer()
	{
		ActionName = TEXT("ScanPlayer");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, false);
		Effects.Add(TTTankGOAPKeys::HasTarget, true);
		Effects.Add(TTTankGOAPKeys::TargetInScanRange, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			return TankAI->ScanForPlayer();
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		const UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain);
		return TankAI && TankAI->HasValidTarget();
	}
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_Chase : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_Chase()
	{
		ActionName = TEXT("ChaseTarget");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, false);
		Preconditions.Add(TTTankGOAPKeys::HasTarget, true);
		Effects.Add(TTTankGOAPKeys::TargetInAttackRange, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			TankAI->MoveToTarget(DeltaTime);
			return true;
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		const UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain);
		return TankAI && TankAI->IsTargetInAttackRange();
	}
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_Aim : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_Aim()
	{
		ActionName = TEXT("AimAtTarget");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, false);
		Preconditions.Add(TTTankGOAPKeys::HasTarget, true);
		Preconditions.Add(TTTankGOAPKeys::TargetInAttackRange, true);
		Effects.Add(TTTankGOAPKeys::TurretAligned, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			TankAI->AimAtTarget();
			return true;
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		const UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain);
		return TankAI && TankAI->IsTurretAligned();
	}
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_Fire : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_Fire()
	{
		ActionName = TEXT("FireAtTarget");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, false);
		Preconditions.Add(TTTankGOAPKeys::HasTarget, true);
		Preconditions.Add(TTTankGOAPKeys::TargetInAttackRange, true);
		Preconditions.Add(TTTankGOAPKeys::TurretAligned, true);
		Preconditions.Add(TTTankGOAPKeys::CanFire, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			UE_LOG(LogTemp, Warning, TEXT("GOAP Action Perform: FireAtTarget"));
			TankAI->FireAtTarget();
			return true;
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		return true;
	}
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_FindHealthPickup : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_FindHealthPickup()
	{
		ActionName = TEXT("FindHealthPickup");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, true);
		Effects.Add(TTTankGOAPKeys::HasHealthPickup, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			UE_LOG(LogTemp, Warning, TEXT("GOAP Action Perform: FindHealthPickup"));
			return TankAI->AcquireHealthPickup();
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		const UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain);
		return TankAI && TankAI->HasHealthPickupTarget();
	}
};

UCLASS()
class BATTLEBLASTER_API UTT_TankAction_MoveToHealthPickup : public UTT_GOAPAction
{
	GENERATED_BODY()

public:
	UTT_TankAction_MoveToHealthPickup()
	{
		ActionName = TEXT("MoveToHealthPickup");
		Preconditions.Add(TTTankGOAPKeys::LowHealth, true);
		Preconditions.Add(TTTankGOAPKeys::HasHealthPickup, true);
		Effects.Add(TTTankGOAPKeys::ReachedHealthPickup, true);
	}

	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime) override
	{
		if (UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain))
		{
			TankAI->MoveToHealthPickup(DeltaTime);
			return true;
		}
		return false;
	}

	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const override
	{
		const UTT_TankAIComponent* TankAI = Cast<UTT_TankAIComponent>(Brain);
		return TankAI && (TankAI->ReachedHealthPickup() || TankAI->IsRecoveredEnough());
	}
};