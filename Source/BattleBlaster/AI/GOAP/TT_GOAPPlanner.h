#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TT_GOAPPlanner.generated.h"

class UTT_GOAPAction;

USTRUCT()
struct FTT_GOAPNode
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UTT_GOAPAction*> Plan;

	UPROPERTY()
	TMap<FName, bool> State;

	UPROPERTY()
	float RunningCost = 0.f;
};

UCLASS()
class BATTLEBLASTER_API UTT_GOAPPlanner : public UObject
{
	GENERATED_BODY()

public:
	bool MakePlan(
		const TArray<UTT_GOAPAction*>& Actions,
		const TMap<FName, bool>& WorldState,
		const TMap<FName, bool>& GoalState,
		TArray<UTT_GOAPAction*>& OutPlan);

private:
	bool BuildGraph(
		TArray<FTT_GOAPNode>& Leaves,
		const FTT_GOAPNode& Parent,
		TArray<UTT_GOAPAction*> UsableActions,
		const TMap<FName, bool>& GoalState);

	bool InState(const TMap<FName, bool>& Test, const TMap<FName, bool>& State) const;
	TMap<FName, bool> ApplyState(const TMap<FName, bool>& Current, const TMap<FName, bool>& Effects) const;
};