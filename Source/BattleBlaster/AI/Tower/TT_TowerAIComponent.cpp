// TT_TowerAIComponent.cpp
#include "BattleBlaster/AI/Tower/TT_TowerAIComponent.h"

#include "BattleBlaster/Commands/Aim/TT_AimCommand.h"
#include "BattleBlaster/Commands/Fire/TT_FireCommand.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

namespace TTTowerGOAPKeys
{
    const FName HasTarget(TEXT("HasTarget"));
    const FName TargetInRange(TEXT("TargetInRange"));
    const FName HasLineOfSight(TEXT("HasLineOfSight"));
    const FName TurretAligned(TEXT("TurretAligned"));
    const FName CanFire(TEXT("CanFire"));
    const FName AttackComplete(TEXT("AttackComplete"));
}

UTT_TowerAction_Scan::UTT_TowerAction_Scan()
{
    ActionName = TEXT("TowerScan");

    Effects.Add(TTTowerGOAPKeys::HasTarget, true);
}

bool UTT_TowerAction_Scan::Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime)
{
    UTT_TowerAIComponent* TowerAI = Cast<UTT_TowerAIComponent>(Brain);
    return TowerAI && TowerAI->ScanForPlayer();
}

bool UTT_TowerAction_Scan::IsDone(UTT_GOAPBrainComponent* Brain) const
{
    const UTT_TowerAIComponent* TowerAI = Cast<UTT_TowerAIComponent>(Brain);
    return TowerAI && TowerAI->HasValidTarget();
}


UTT_TowerAction_CheckTarget::UTT_TowerAction_CheckTarget()
{
    ActionName = TEXT("TowerCheckTarget");
    Preconditions.Add(TTTowerGOAPKeys::HasTarget, true);

    Effects.Add(TTTowerGOAPKeys::TargetInRange, true);
    Effects.Add(TTTowerGOAPKeys::HasLineOfSight, true);
}

bool UTT_TowerAction_CheckTarget::Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime)
{
    const UTT_TowerAIComponent* TowerAI = Cast<UTT_TowerAIComponent>(Brain);
    return TowerAI && TowerAI->IsTargetInRange() && TowerAI->HasLineOfSightToTarget();
}

bool UTT_TowerAction_CheckTarget::IsDone(UTT_GOAPBrainComponent* Brain) const
{
    const UTT_TowerAIComponent* TowerAI = Cast<UTT_TowerAIComponent>(Brain);
    return TowerAI && TowerAI->IsTargetInRange() && TowerAI->HasLineOfSightToTarget();
}


UTT_TowerAction_Aim::UTT_TowerAction_Aim()
{
    ActionName = TEXT("TowerAim");
    Preconditions.Add(TTTowerGOAPKeys::HasTarget, true);
    Preconditions.Add(TTTowerGOAPKeys::TargetInRange, true);
    Effects.Add(TTTowerGOAPKeys::TurretAligned, true);
}

bool UTT_TowerAction_Aim::Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime)
{
    UTT_TowerAIComponent* TowerAI = Cast<UTT_TowerAIComponent>(Brain);
    if (!TowerAI) return false;

    TowerAI->AimAtTarget();
    return true; 
}

bool UTT_TowerAction_Aim::IsDone(UTT_GOAPBrainComponent* Brain) const
{
    const UTT_TowerAIComponent* TowerAI = Cast<UTT_TowerAIComponent>(Brain);
    return TowerAI && TowerAI->IsTurretAligned();
}


UTT_TowerAction_Fire::UTT_TowerAction_Fire()
{
    ActionName = TEXT("TowerFire");
    Preconditions.Add(TTTowerGOAPKeys::HasTarget, true);
    Preconditions.Add(TTTowerGOAPKeys::TargetInRange, true);
    Preconditions.Add(TTTowerGOAPKeys::HasLineOfSight, true);
    Preconditions.Add(TTTowerGOAPKeys::TurretAligned, true);
    Preconditions.Add(TTTowerGOAPKeys::CanFire, true);
    Effects.Add(TTTowerGOAPKeys::AttackComplete, true);
}

bool UTT_TowerAction_Fire::Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime)
{
    UTT_TowerAIComponent* TowerAI = Cast<UTT_TowerAIComponent>(Brain);
    return TowerAI && TowerAI->FireAtTarget();
}

bool UTT_TowerAction_Fire::IsDone(UTT_GOAPBrainComponent* Brain) const
{

    return true;
}


UTT_TowerAIComponent::UTT_TowerAIComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTT_TowerAIComponent::BeginPlay()
{
    Super::BeginPlay();


    mOwnerPawn = Cast<ATT_BasePawn>(GetOwner());
    ensureMsgf(mOwnerPawn, TEXT("UTT_TowerAIComponent must be attached to an ATT_BasePawn"));
}

void UTT_TowerAIComponent::BuildActions()
{
    Actions.Add(NewObject<UTT_TowerAction_Scan>(this));
    Actions.Add(NewObject<UTT_TowerAction_CheckTarget>(this));
    Actions.Add(NewObject<UTT_TowerAction_Aim>(this));
    Actions.Add(NewObject<UTT_TowerAction_Fire>(this));
}

TMap<FName, bool> UTT_TowerAIComponent::BuildWorldState() const
{
    TMap<FName, bool> State;
    State.Add(TTTowerGOAPKeys::HasTarget, HasValidTarget());
    State.Add(TTTowerGOAPKeys::TargetInRange, IsTargetInRange());
    State.Add(TTTowerGOAPKeys::HasLineOfSight, HasLineOfSightToTarget());
    State.Add(TTTowerGOAPKeys::TurretAligned, IsTurretAligned());
    State.Add(TTTowerGOAPKeys::CanFire, CanFire());
    State.Add(TTTowerGOAPKeys::AttackComplete, false); 

    if (GEngine && CurrentTarget && mOwnerPawn)
    {
        GEngine->AddOnScreenDebugMessage(
            (int32)((PTRINT)this & 0x7fffffff),
            0.f,
            FColor::Yellow,
            FString::Printf(
                TEXT("TowerDist: %.1f | LOS:%d Align:%d CanFire:%d"),
                FVector::Dist(mOwnerPawn->GetActorLocation(), CurrentTarget->GetActorLocation()),
                State[TTTowerGOAPKeys::HasLineOfSight],
                State[TTTowerGOAPKeys::TurretAligned],
                State[TTTowerGOAPKeys::CanFire]
            )
        );
    }

    return State;
}

TMap<FName, bool> UTT_TowerAIComponent::BuildGoalState() const
{
    TMap<FName, bool> Goal;
    Goal.Add(TTTowerGOAPKeys::AttackComplete, true);
    return Goal;
}

bool UTT_TowerAIComponent::ShouldReplan() const
{
    return !HasValidTarget() || !IsTargetInRange();
}


bool UTT_TowerAIComponent::ScanForPlayer()
{
    if (!mOwnerPawn) return false;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        CurrentTarget = nullptr;
        return false;
    }

    const float DistSq = FVector::DistSquared(
        PlayerPawn->GetActorLocation(),
        mOwnerPawn->GetActorLocation()
    );

    if (DistSq <= FMath::Square(ScanRange))
    {
        CurrentTarget = PlayerPawn;
        return true;
    }

    CurrentTarget = nullptr;
    return false;
}

bool UTT_TowerAIComponent::HasValidTarget() const
{
    return IsValid(CurrentTarget);
}

bool UTT_TowerAIComponent::IsTargetInRange() const
{
    if (!CurrentTarget || !mOwnerPawn) return false;

    const float DistSq = FVector::DistSquared(
        CurrentTarget->GetActorLocation(),
        mOwnerPawn->GetActorLocation()
    );
    return DistSq <= FMath::Square(FireRange);
}

bool UTT_TowerAIComponent::HasLineOfSightToTarget() const
{
    if (!CurrentTarget || !mOwnerPawn || !mOwnerPawn->ProjectileSpawnPoint)
    {
        return false;
    }

    const FVector TowerLocation = mOwnerPawn->GetActorLocation();
    const FVector TargetLocation = CurrentTarget->GetActorLocation();

    constexpr float GuaranteedCloseRangeLOS = 1300.f;
    const float TowerToTargetDistSq = FVector::DistSquared(TowerLocation, TargetLocation);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            0.f,
            FColor::Green,
            FString::Printf(TEXT("%s TowerToTargetDist: %.1f"),
                *GetOwner()->GetName(),
                FVector::Dist(TowerLocation, TargetLocation))
        );
    }

    if (TowerToTargetDistSq <= FMath::Square(GuaranteedCloseRangeLOS))
    {
        return true;
    }

    const FVector Start = mOwnerPawn->ProjectileSpawnPoint->GetComponentLocation();

    FVector Origin;
    FVector Extents;
    CurrentTarget->GetActorBounds(true, Origin, Extents);

    FCollisionQueryParams Params(SCENE_QUERY_STAT(TowerLOS), false);
    Params.AddIgnoredActor(mOwnerPawn);
    Params.AddIgnoredActor(CurrentTarget);

    // Ignore other towers so they do not block this tower's LOS test.
    TArray<AActor*> TowerActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), mOwnerPawn->GetClass(), TowerActors);
    for (AActor* TowerActor : TowerActors)
    {
        if (TowerActor)
        {
            Params.AddIgnoredActor(TowerActor);
        }
    }

    TArray<FVector> TestPoints;
    TestPoints.Add(Origin);
    TestPoints.Add(Origin + FVector(0.f, 0.f, Extents.Z * 0.75f));
    TestPoints.Add(Origin + FVector(Extents.X * 0.5f, 0.f, 0.f));
    TestPoints.Add(Origin - FVector(Extents.X * 0.5f, 0.f, 0.f));
    TestPoints.Add(Origin + FVector(0.f, Extents.Y * 0.5f, 0.f));
    TestPoints.Add(Origin - FVector(0.f, Extents.Y * 0.5f, 0.f));

    constexpr float LOSProbeRadius = 20.f;

    for (const FVector& End : TestPoints)
    {
        FHitResult Hit;
        const bool bHitSomething = GetWorld()->SweepSingleByChannel(
            Hit,
            Start,
            End,
            FQuat::Identity,
            ECC_Visibility,
            FCollisionShape::MakeSphere(LOSProbeRadius),
            Params
        );

        // If we hit nothing, LOS is clear.
        // If we ignored the target and still hit something, that thing blocks LOS.
        const bool bThisPathHasLOS = !bHitSomething;

        DrawDebugLine(
            GetWorld(),
            Start,
            End,
            bThisPathHasLOS ? FColor::Green : FColor::Red,
            false,
            0.05f,
            0,
            2.f
        );

        if (bThisPathHasLOS)
        {
            return true;
        }
    }

    return false;
}

bool UTT_TowerAIComponent::IsTurretAligned() const
{
    if (!CurrentTarget || !mOwnerPawn || !mOwnerPawn->TurretMeshComponent)
    {
        bWasAlignedLastFrame = false;
        return false;
    }

    const FVector TurretLocation = mOwnerPawn->TurretMeshComponent->GetComponentLocation();
    const FVector AimPoint = GetAimPoint();
    const FVector ToTargetRaw = AimPoint - TurretLocation;

    // Edge case: target is basically on top of the turret pivot.
    // In that case, angle becomes noisy 
    constexpr float NearZeroDistance = 80.f;
    const float DistSq = ToTargetRaw.SizeSquared();
    if (DistSq <= FMath::Square(NearZeroDistance))
    {
        bWasAlignedLastFrame = true;
        return true;
    }

    const FVector ToTarget = ToTargetRaw.GetSafeNormal();
    const FVector Forward = mOwnerPawn->TurretMeshComponent->GetForwardVector().GetSafeNormal();

    if (ToTarget.IsNearlyZero() || Forward.IsNearlyZero())
    {
        bWasAlignedLastFrame = false;
        return false;
    }

    const float Dot = FVector::DotProduct(Forward, ToTarget);
    const float ClampedDot = FMath::Clamp(Dot, -1.f, 1.f);
    const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(ClampedDot));

    const float TowerToTargetDist = FVector::Dist(
        mOwnerPawn->GetActorLocation(),
        CurrentTarget->GetActorLocation()
    );

    // close range => more forgiving
    // long range  => tighter
    //
    // 0 units     -> 22 deg
    // 2000 units  -> AimToleranceDegrees
    const float BaseTolerance = FMath::GetMappedRangeValueClamped(
        FVector2D(0.f, 2000.f),
        FVector2D(22.f, AimToleranceDegrees),
        TowerToTargetDist
    );

    // Hysteresis:
    // once aligned, allow a slightly larger angle before dropping out.
    const float EnterTolerance = BaseTolerance;
    const float ExitTolerance = BaseTolerance + 2.5f;

    const bool bAlignedNow = bWasAlignedLastFrame
        ? (AngleDegrees <= ExitTolerance)
        : (AngleDegrees <= EnterTolerance);

    bWasAlignedLastFrame = bAlignedNow;
    return bAlignedNow;
}

bool UTT_TowerAIComponent::CanFire() const
{
    return (GetWorld()->GetTimeSeconds() - LastFireTime) >= FireCooldown;
}

void UTT_TowerAIComponent::AimAtTarget()
{
    if (!mOwnerPawn || !CurrentTarget) return;

    TT_AimCommand AimCmd;
    const FInputActionValue Dummy;
    AimCmd.Execute(mOwnerPawn, Dummy, GetAimPoint(), AimSpeed);
}

bool UTT_TowerAIComponent::FireAtTarget()
{
    if (!mOwnerPawn || !CurrentTarget || !CanFire())
    {
        return false;
    }

    TT_FireCommand FireCmd;
    const FInputActionValue Dummy{};
    FireCmd.Execute(mOwnerPawn, Dummy, GetAimPoint(), AimSpeed);

    LastFireTime = GetWorld()->GetTimeSeconds();
    return true;
}

FVector UTT_TowerAIComponent::GetAimPoint() const
{
    if (!CurrentTarget)
    {
        return FVector::ZeroVector;
    }

    FVector Origin;
    FVector Extents;
    CurrentTarget->GetActorBounds(true, Origin, Extents);
    return Origin;
}