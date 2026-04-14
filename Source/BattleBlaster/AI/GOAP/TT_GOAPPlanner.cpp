#include "BattleBlaster/AI/GOAP/TT_GOAPPlanner.h"
#include "BattleBlaster/AI/GOAP/TT_GOAPAction.h"

bool UTT_GOAPPlanner::MakePlan(
	const TArray<UTT_GOAPAction*>& Actions,
	const TMap<FName, bool>& WorldState,
	const TMap<FName, bool>& GoalState,
	TArray<UTT_GOAPAction*>& OutPlan)
{
	OutPlan.Empty();

	FTT_GOAPNode Start;
	Start.State = WorldState;
	Start.RunningCost = 0.f;

	TArray<FTT_GOAPNode> Leaves;
	const bool bFound = BuildGraph(Leaves, Start, Actions, GoalState);
	if (!bFound || Leaves.Num() == 0)
	{
		return false;
	}

	const FTT_GOAPNode* Cheapest = &Leaves[0];
	for (const FTT_GOAPNode& Leaf : Leaves)
	{
		if (Leaf.RunningCost < Cheapest->RunningCost)
		{
			Cheapest = &Leaf;
		}
	}

	OutPlan = Cheapest->Plan;
	return OutPlan.Num() > 0;
}

bool UTT_GOAPPlanner::BuildGraph(
	TArray<FTT_GOAPNode>& Leaves,
	const FTT_GOAPNode& Parent,
	TArray<UTT_GOAPAction*> UsableActions,
	const TMap<FName, bool>& GoalState)
{
	bool bFoundOne = false;

	for (int32 i = 0; i < UsableActions.Num(); ++i)
	{
		UTT_GOAPAction* Action = UsableActions[i];
		if (!Action)
		{
			continue;
		}

		if (!InState(Action->Preconditions, Parent.State))
		{
			continue;
		}

		FTT_GOAPNode Node;
		Node.Plan = Parent.Plan;
		Node.Plan.Add(Action);
		Node.State = ApplyState(Parent.State, Action->Effects);
		Node.RunningCost = Parent.RunningCost + Action->Cost;

		if (InState(GoalState, Node.State))
		{
			Leaves.Add(Node);
			bFoundOne = true;
		}
		else
		{
			TArray<UTT_GOAPAction*> Subset = UsableActions;
			Subset.RemoveAt(i);

			if (BuildGraph(Leaves, Node, Subset, GoalState))
			{
				bFoundOne = true;
			}
		}
	}

	return bFoundOne;
}

bool UTT_GOAPPlanner::InState(const TMap<FName, bool>& Test, const TMap<FName, bool>& State) const
{
	for (const TPair<FName, bool>& Pair : Test)
	{
		const bool* Found = State.Find(Pair.Key);
		if (!Found || *Found != Pair.Value)
		{
			return false;
		}
	}
	return true;
}

TMap<FName, bool> UTT_GOAPPlanner::ApplyState(const TMap<FName, bool>& Current, const TMap<FName, bool>& Effects) const
{
	TMap<FName, bool> NewState = Current;
	for (const TPair<FName, bool>& Pair : Effects)
	{
		NewState.FindOrAdd(Pair.Key) = Pair.Value;
	}
	return NewState;
}