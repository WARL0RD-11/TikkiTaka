#include "BattleBlaster/AI/GOAP/TT_GOAPBrainComponent.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPPlanner.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPAction.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"

UTT_GOAPBrainComponent::UTT_GOAPBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTT_GOAPBrainComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<ATT_BasePawn>(GetOwner());
	if (!OwnerPawn)
	{
		SetComponentTickEnabled(false);
		return;
	}

	Planner = NewObject<UTT_GOAPPlanner>(this);
	BuildActions();
	RebuildPlan();
}

void UTT_GOAPBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerPawn || !Planner)
	{
		return;
	}

	if (ShouldReplan() || CurrentPlan.Num() == 0)
	{
		RebuildPlan();
	}

	if (CurrentPlan.Num() == 0)
	{
		return;
	}

	UTT_GOAPAction* Action = CurrentPlan[0];
	if (!Action)
	{
		CurrentPlan.RemoveAt(0);
		return;
	}

	if (!Action->CheckProceduralPrecondition(this))
	{
		RebuildPlan();
		return;
	}

	const bool bSucceeded = Action->Perform(this, DeltaTime);
	if (!bSucceeded)
	{
		RebuildPlan();
		return;
	}

	if (Action->IsDone(this))
	{
		CurrentPlan.RemoveAt(0);

		if (CurrentPlan.Num() == 0)
		{
			RebuildPlan();
		}
	}
}

bool UTT_GOAPBrainComponent::ShouldReplan() const
{
	return false;
}

void UTT_GOAPBrainComponent::RebuildPlan()
{
	CurrentPlan.Empty();

	TArray<UTT_GOAPAction*> Usable;
	for (UTT_GOAPAction* Action : Actions)
	{
		if (Action)
		{
			Action->ResetAction(this);
			Usable.Add(Action);
		}
	}

	if (Planner)
	{
		Planner->MakePlan(Usable, BuildWorldState(), BuildGoalState(), CurrentPlan);
	}
}