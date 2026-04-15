#include "TT_TankAIComponent.h"

#include "BattleBlaster/Commands/Move/TT_MoveCommand.h"
#include "BattleBlaster/Commands/Aim/TT_AimCommand.h"
#include "BattleBlaster/Commands/Fire/TT_FireCommand.h"
#include "BattleBlaster/Pawn/Tank/TT_EnemyTank.h"
#include "BattleBlaster/Pickups/TT_HealthPickup.h"
#include "BattleBlaster/Components/TT_HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputActionValue.h"
#include "Components/ArrowComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

UTT_TankAIComponent::UTT_TankAIComponent()
{
}

void UTT_TankAIComponent::BuildActions()
{
	Actions.Empty();

	Actions.Add(NewObject<UTT_TankAction_FindPatrolPoint>(this));
	Actions.Add(NewObject<UTT_TankAction_MoveToPatrol>(this));

	Actions.Add(NewObject<UTT_TankAction_ScanPlayer>(this));
	Actions.Add(NewObject<UTT_TankAction_Chase>(this));
	Actions.Add(NewObject<UTT_TankAction_Aim>(this));
	Actions.Add(NewObject<UTT_TankAction_Fire>(this));

	Actions.Add(NewObject<UTT_TankAction_FindHealthPickup>(this));
	Actions.Add(NewObject<UTT_TankAction_MoveToHealthPickup>(this));
}

void UTT_TankAIComponent::ClearInvalidTargets()
{
	if (TargetHealthPickup && !TargetHealthPickup->IsPickupActive())
	{
		TargetHealthPickup = nullptr;
	}

	if (CurrentTarget)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!PlayerPawn || CurrentTarget != PlayerPawn)
		{
			CurrentTarget = nullptr;
		}
	}
}

void UTT_TankAIComponent::UpdateCommitmentFlags()
{
	ClearInvalidTargets();

	if (IsLowHealth())
	{
		bCommittedToRecovery = true;
		bCommittedToAttack = false;
		return;
	}

	if (bCommittedToRecovery && IsRecoveredEnough())
	{
		bCommittedToRecovery = false;
		TargetHealthPickup = nullptr;
	}

	if (HasValidTarget() || IsTargetInScanRange())
	{
		bCommittedToAttack = true;
	}
	else
	{
		bCommittedToAttack = false;
	}
}

TMap<FName, bool> UTT_TankAIComponent::BuildWorldState() const
{
	UTT_TankAIComponent* MutableThis = const_cast<UTT_TankAIComponent*>(this);
	MutableThis->UpdateCommitmentFlags();

	TMap<FName, bool> State;
	State.Add(TTTankGOAPKeys::HasTarget, HasValidTarget());
	State.Add(TTTankGOAPKeys::TargetInScanRange, IsTargetInScanRange());
	State.Add(TTTankGOAPKeys::TargetInAttackRange, IsTargetInAttackRange());
	State.Add(TTTankGOAPKeys::LowHealth, IsLowHealth());
	State.Add(TTTankGOAPKeys::HasHealthPickup, HasHealthPickupTarget());
	State.Add(TTTankGOAPKeys::ReachedHealthPickup, ReachedHealthPickup());
	State.Add(TTTankGOAPKeys::HasPatrolPoint, HasPatrolTarget());
	State.Add(TTTankGOAPKeys::ReachedPatrolPoint, ReachedPatrolPoint());
	State.Add(TTTankGOAPKeys::CanFire, CanFire());
	State.Add(TTTankGOAPKeys::TurretAligned, IsTurretAligned());
	State.Add(TTTankGOAPKeys::WantsAttack, bCommittedToAttack);
	State.Add(TTTankGOAPKeys::WantsRecovery, bCommittedToRecovery);

	MutableThis->TryFireReactive();

	if (bDebugAI)
	{
		MutableThis->DebugDrawAIState();
	}

	return State;
}

TMap<FName, bool> UTT_TankAIComponent::BuildGoalState() const
{
	TMap<FName, bool> Goal;

	if (IsLowHealth() || bCommittedToRecovery)
	{
		Goal.Add(TTTankGOAPKeys::ReachedHealthPickup, true);
		return Goal;
	}

	if (HasValidTarget() || IsTargetInScanRange() || bCommittedToAttack)
	{
		if (!HasValidTarget())
		{
			Goal.Add(TTTankGOAPKeys::HasTarget, true);
		}
		else if (!IsTargetInAttackRange())
		{
			Goal.Add(TTTankGOAPKeys::TargetInAttackRange, true);
		}
		else if (!IsTurretAligned())
		{
			Goal.Add(TTTankGOAPKeys::TurretAligned, true);
		}
		else
		{
			// Once we are in firing posture, GOAP has done its job.
			// Actual firing should happen reactively.
			Goal.Add(TTTankGOAPKeys::TurretAligned, true);
		}

		return Goal;
	}

	if (!HasPatrolTarget())
	{
		Goal.Add(TTTankGOAPKeys::HasPatrolPoint, true);
	}
	else
	{
		Goal.Add(TTTankGOAPKeys::ReachedPatrolPoint, true);
	}

	return Goal;
}

bool UTT_TankAIComponent::ShouldReplan() const
{
	if (IsLowHealth())
	{
		if (!bCommittedToRecovery)
		{
			return true;
		}

		if (!HasHealthPickupTarget())
		{
			return true;
		}

		if (TargetHealthPickup && !TargetHealthPickup->IsPickupActive())
		{
			return true;
		}

		if (!ReachedHealthPickup())
		{
			return true;
		}

		return false;
	}

	if (bCommittedToRecovery && IsRecoveredEnough())
	{
		return true;
	}

	if (HasValidTarget() || IsTargetInScanRange())
	{
		if (!bCommittedToAttack)
		{
			return true;
		}

		if (!HasValidTarget() && IsTargetInScanRange())
		{
			return true;
		}

		if (HasValidTarget() && !IsTargetInAttackRange())
		{
			return true;
		}

		if (HasValidTarget() && IsTargetInAttackRange() && !IsTurretAligned())
		{
			return true;
		}

		return false;
	}

	if (bCommittedToAttack && !HasValidTarget() && !IsTargetInScanRange())
	{
		return true;
	}

	if (!HasPatrolTarget())
	{
		return true;
	}

	if (ReachedPatrolPoint() && !IsTargetInScanRange())
	{
		return true;
	}

	return false;
}

bool UTT_TankAIComponent::HasValidTarget() const
{
	return IsValid(CurrentTarget);
}

bool UTT_TankAIComponent::IsTargetInScanRange() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || !OwnerPawn)
	{
		return false;
	}

	return FVector::DistSquared(PlayerPawn->GetActorLocation(), OwnerPawn->GetActorLocation()) <= FMath::Square(ScanRange);
}

bool UTT_TankAIComponent::IsTargetInAttackRange() const
{
	return CurrentTarget &&
		OwnerPawn &&
		FVector::DistSquared(CurrentTarget->GetActorLocation(), OwnerPawn->GetActorLocation()) <= FMath::Square(AttackRange);
}

bool UTT_TankAIComponent::CanFire() const
{
	return GetWorld() && (GetWorld()->GetTimeSeconds() - LastFireTime) >= FireCooldown;
}

bool UTT_TankAIComponent::IsLowHealth() const
{
	return OwnerPawn && OwnerPawn->HealthComponent &&
		OwnerPawn->HealthComponent->GetHealthPercent() <= LowHealthThreshold;
}

bool UTT_TankAIComponent::IsRecoveredEnough() const
{
	return OwnerPawn && OwnerPawn->HealthComponent &&
		OwnerPawn->HealthComponent->GetHealthPercent() >= RecoverHealthThreshold;
}

bool UTT_TankAIComponent::HasHealthPickupTarget() const
{
	return IsValid(TargetHealthPickup) && TargetHealthPickup->IsPickupActive();
}

bool UTT_TankAIComponent::ReachedHealthPickup() const
{
	return TargetHealthPickup &&
		OwnerPawn &&
		FVector::DistSquared(TargetHealthPickup->GetActorLocation(), OwnerPawn->GetActorLocation()) <= FMath::Square(PickupAcceptanceRadius);
}

bool UTT_TankAIComponent::HasPatrolTarget() const
{
	return IsValid(CurrentPatrolPoint);
}

bool UTT_TankAIComponent::ReachedPatrolPoint() const
{
	return CurrentPatrolPoint &&
		OwnerPawn &&
		FVector::DistSquared(CurrentPatrolPoint->GetActorLocation(), OwnerPawn->GetActorLocation()) <= FMath::Square(PatrolPointAcceptanceRadius);
}

bool UTT_TankAIComponent::IsTurretAligned() const
{
	if (!CurrentTarget || !OwnerPawn || !OwnerPawn->TurretMeshComponent)
	{
		return false;
	}

	const FVector TurretLocation = OwnerPawn->TurretMeshComponent->GetComponentLocation();

	FVector ToTarget = GetAimPoint() - TurretLocation;
	ToTarget.Z = 0.f;

	if (ToTarget.IsNearlyZero())
	{
		return true;
	}

	const float DesiredYaw = ToTarget.Rotation().Yaw;
	const float CurrentYaw = OwnerPawn->TurretMeshComponent->GetComponentRotation().Yaw;
	const float DeltaYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw));

	return DeltaYaw <= FireAngleTolerance;
}

bool UTT_TankAIComponent::ScanForPlayer()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || !OwnerPawn)
	{
		CurrentTarget = nullptr;
		return false;
	}

	const float DistSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), OwnerPawn->GetActorLocation());

	if (DistSq <= FMath::Square(ScanRange))
	{
		CurrentTarget = PlayerPawn;
		LastSeenTargetTime = GetWorld()->GetTimeSeconds();
		return true;
	}

	if (CurrentTarget && (GetWorld()->GetTimeSeconds() - LastSeenTargetTime) <= LostTargetGraceTime)
	{
		return true;
	}

	CurrentTarget = nullptr;
	return false;
}

bool UTT_TankAIComponent::AcquireHealthPickup()
{
	if (!OwnerPawn)
	{
		return false;
	}

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATT_HealthPickup::StaticClass(), Found);

	float BestDistSq = TNumericLimits<float>::Max();
	ATT_HealthPickup* BestPickup = nullptr;

	for (AActor* Actor : Found)
	{
		ATT_HealthPickup* Pickup = Cast<ATT_HealthPickup>(Actor);
		if (!Pickup || !Pickup->IsPickupActive())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(Pickup->GetActorLocation(), OwnerPawn->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestPickup = Pickup;
		}
	}

	TargetHealthPickup = BestPickup;
	return TargetHealthPickup != nullptr;
}

bool UTT_TankAIComponent::ChoosePatrolPoint()
{
	if (PatrolPoints.Num() == 0)
	{
		CurrentPatrolPoint = nullptr;
		return false;
	}

	for (int32 TryIndex = 0; TryIndex < PatrolPoints.Num(); ++TryIndex)
	{
		PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();

		if (IsValid(PatrolPoints[PatrolIndex]))
		{
			CurrentPatrolPoint = PatrolPoints[PatrolIndex];
			return true;
		}
	}

	CurrentPatrolPoint = nullptr;
	return false;
}

void UTT_TankAIComponent::MoveToTarget(float DeltaTime)
{
	if (!CurrentTarget)
	{
		return;
	}

	if (ATT_EnemyTank* Tank = GetTankOwner())
	{
		const float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Tank->GetActorLocation());

		if (DistSq > FMath::Square(AttackRange * 0.9f))
		{
			TT_MoveCommand MoveCmd;
			MoveCmd.ExecuteMoveToLocation(Tank, CurrentTarget->GetActorLocation(), DeltaTime, Tank->MoveSpeed, Tank->TurnSpeed);
		}
	}
}

void UTT_TankAIComponent::MoveToHealthPickup(float DeltaTime)
{
	if (!TargetHealthPickup)
	{
		return;
	}

	if (ATT_EnemyTank* Tank = GetTankOwner())
	{
		TT_MoveCommand MoveCmd;
		MoveCmd.ExecuteMoveToLocation(Tank, TargetHealthPickup->GetActorLocation(), DeltaTime, Tank->MoveSpeed, Tank->TurnSpeed);
	}
}

void UTT_TankAIComponent::MoveToPatrolPoint(float DeltaTime)
{
	if (!CurrentPatrolPoint)
	{
		return;
	}

	if (ATT_EnemyTank* Tank = GetTankOwner())
	{
		TT_MoveCommand MoveCmd;
		MoveCmd.ExecuteMoveToLocation(Tank, CurrentPatrolPoint->GetActorLocation(), DeltaTime, Tank->MoveSpeed, Tank->TurnSpeed);
	}
}

void UTT_TankAIComponent::AimAtTarget()
{
	if (!OwnerPawn || !CurrentTarget)
	{
		return;
	}

	TT_AimCommand AimCmd;
	const FInputActionValue Dummy;
	AimCmd.Execute(OwnerPawn, Dummy, GetAimPoint(), AimSpeed);
}

void UTT_TankAIComponent::FireAtTarget()
{
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("TankAI Fire blocked: OwnerPawn invalid"));
		return;
	}

	if (!CurrentTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("TankAI Fire blocked: CurrentTarget invalid"));
		return;
	}

	if (!CanFire())
	{
		UE_LOG(LogTemp, Warning, TEXT("TankAI Fire blocked: Cooldown"));
		return;
	}

	if (!IsTargetInAttackRange())
	{
		UE_LOG(LogTemp, Warning, TEXT("TankAI Fire blocked: Target not in range"));
		return;
	}

	if (!IsTurretAligned())
	{
		UE_LOG(LogTemp, Warning, TEXT("TankAI Fire blocked: Turret not aligned"));
		return;
	}

	ATT_EnemyTank* Tank = GetTankOwner();
	if (!Tank)
	{
		UE_LOG(LogTemp, Error, TEXT("TankAI Fire blocked: GetTankOwner() returned null"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("TankAI Fire: OwnerPawn=%s | Tank=%s"),
		*OwnerPawn->GetName(),
		*Tank->GetName());

	// If ATT_EnemyTank inherits from ATT_BasePawn and has these inherited members,
	// this will immediately tell us whether the fire command has what it needs.
	UE_LOG(LogTemp, Warning, TEXT("TankAI Fire: TurretMesh=%s"),
		Tank->TurretMeshComponent ? *Tank->TurretMeshComponent->GetName() : TEXT("NULL"));

	UE_LOG(LogTemp, Warning, TEXT("TankAI Fire: ProjectileSpawnPoint=%s"),
		Tank->ProjectileSpawnPoint ? *Tank->ProjectileSpawnPoint->GetName() : TEXT("NULL"));

	if (Tank->ProjectileSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("TankAI Fire: SpawnLocation=%s | SpawnRotation=%s"),
			*Tank->ProjectileSpawnPoint->GetComponentLocation().ToString(),
			*Tank->ProjectileSpawnPoint->GetComponentRotation().ToString());
	}

	TT_FireCommand FireCmd;
	const FInputActionValue Dummy;
	FireCmd.Execute(OwnerPawn, Dummy, GetAimPoint(), AimSpeed);

	UE_LOG(LogTemp, Warning, TEXT("TankAI Fire: FireCommand executed"));

	LastFireTime = GetWorld()->GetTimeSeconds();
}

FVector UTT_TankAIComponent::GetAimPoint() const
{
	return CurrentTarget ? CurrentTarget->GetActorLocation() : FVector::ZeroVector;
}

ATT_EnemyTank* UTT_TankAIComponent::GetTankOwner() const
{
	return Cast<ATT_EnemyTank>(OwnerPawn);
}

FString UTT_TankAIComponent::GetCurrentGoalString() const
{
	if (IsLowHealth() || bCommittedToRecovery)
	{
		if (!HasHealthPickupTarget())
		{
			return TEXT("RECOVERY: FindHealthPickup");
		}

		if (!ReachedHealthPickup() && !IsRecoveredEnough())
		{
			return TEXT("RECOVERY: MoveToHealthPickup");
		}

		return TEXT("RECOVERY: WaitingForHeal");
	}

	if (HasValidTarget() || IsTargetInScanRange() || bCommittedToAttack)
	{
		if (!HasValidTarget())
		{
			return TEXT("COMBAT: ScanForTarget");
		}

		if (!IsTargetInAttackRange())
		{
			return TEXT("COMBAT: ChaseTarget");
		}

		if (!IsTurretAligned())
		{
			return TEXT("COMBAT: AimAtTarget");
		}

		if (!CanFire())
		{
			return TEXT("COMBAT: HoldAim_Cooldown");
		}

		return TEXT("COMBAT: FireAtTarget");
	}

	if (!HasPatrolTarget())
	{
		return TEXT("PATROL: FindPatrolPoint");
	}

	if (!ReachedPatrolPoint())
	{
		return TEXT("PATROL: MoveToPatrolPoint");
	}

	return TEXT("PATROL: ReachedPatrolPoint");
}

FString UTT_TankAIComponent::GetCurrentDebugStateString() const
{
	const float HealthPct = (OwnerPawn && OwnerPawn->HealthComponent)
		? OwnerPawn->HealthComponent->GetHealthPercent()
		: -1.f;

	const FString GoalString = GetCurrentGoalString();

	const FString TargetString = HasValidTarget() ? TEXT("Yes") : TEXT("No");
	const FString ScanString = IsTargetInScanRange() ? TEXT("Yes") : TEXT("No");
	const FString AttackRangeString = IsTargetInAttackRange() ? TEXT("Yes") : TEXT("No");
	const FString AlignedString = IsTurretAligned() ? TEXT("Yes") : TEXT("No");
	const FString CanFireString = CanFire() ? TEXT("Yes") : TEXT("No");
	const FString LowHealthString = IsLowHealth() ? TEXT("Yes") : TEXT("No");
	const FString HasPickupString = HasHealthPickupTarget() ? TEXT("Yes") : TEXT("No");
	const FString ReachedPickupString = ReachedHealthPickup() ? TEXT("Yes") : TEXT("No");
	const FString HasPatrolString = HasPatrolTarget() ? TEXT("Yes") : TEXT("No");
	const FString ReachedPatrolString = ReachedPatrolPoint() ? TEXT("Yes") : TEXT("No");

	return FString::Printf(
		TEXT("Goal=%s | HP=%.2f | Target=%s | Scan=%s | AttackRange=%s | Aligned=%s | CanFire=%s | LowHP=%s | HasPickup=%s | ReachedPickup=%s | HasPatrol=%s | ReachedPatrol=%s | CommitAttack=%s | CommitRecovery=%s"),
		*GoalString,
		HealthPct,
		*TargetString,
		*ScanString,
		*AttackRangeString,
		*AlignedString,
		*CanFireString,
		*LowHealthString,
		*HasPickupString,
		*ReachedPickupString,
		*HasPatrolString,
		*ReachedPatrolString,
		bCommittedToAttack ? TEXT("Yes") : TEXT("No"),
		bCommittedToRecovery ? TEXT("Yes") : TEXT("No")
	);
}

void UTT_TankAIComponent::DebugDrawAIState() const
{
	if (!bDebugAI || !OwnerPawn || !GetWorld())
	{
		return;
	}

	const FVector PawnLocation = OwnerPawn->GetActorLocation();
	const FVector TextLocation = PawnLocation + FVector(0.f, 0.f, 140.f);

	const FString DebugString = GetCurrentDebugStateString();

	DrawDebugString(
		GetWorld(),
		TextLocation,
		DebugString,
		nullptr,
		FColor::White,
		0.f,
		true
	);

	DrawDebugSphere(
		GetWorld(),
		PawnLocation,
		ScanRange,
		24,
		FColor::Blue,
		false,
		0.f,
		0,
		1.5f
	);

	DrawDebugSphere(
		GetWorld(),
		PawnLocation,
		AttackRange,
		24,
		FColor::Red,
		false,
		0.f,
		0,
		1.5f
	);

	if (CurrentTarget)
	{
		DrawDebugLine(
			GetWorld(),
			PawnLocation + FVector(0.f, 0.f, 40.f),
			CurrentTarget->GetActorLocation() + FVector(0.f, 0.f, 40.f),
			IsTurretAligned() ? FColor::Green : FColor::Yellow,
			false,
			0.f,
			0,
			2.f
		);
	}

	if (TargetHealthPickup)
	{
		DrawDebugLine(
			GetWorld(),
			PawnLocation + FVector(0.f, 0.f, 20.f),
			TargetHealthPickup->GetActorLocation() + FVector(0.f, 0.f, 20.f),
			FColor::Cyan,
			false,
			0.f,
			0,
			2.f
		);
	}
}

void UTT_TankAIComponent::TryFireReactive()
{
	if (!bCommittedToRecovery &&
		HasValidTarget() &&
		IsTargetInAttackRange() &&
		IsTurretAligned() &&
		CanFire())
	{
		UE_LOG(LogTemp, Warning, TEXT("TankAI Reactive Fire: EXECUTING"));
		FireAtTarget();
	}
}