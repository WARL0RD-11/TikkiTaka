#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TT_GOAPBrainComponent.generated.h"

class UTT_GOAPPlanner;
class UTT_GOAPAction;
class ATT_BasePawn;

UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLEBLASTER_API UTT_GOAPBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTT_GOAPBrainComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	TObjectPtr<ATT_BasePawn> OwnerPawn;

	UPROPERTY()
	TObjectPtr<UTT_GOAPPlanner> Planner;

	UPROPERTY()
	TArray<TObjectPtr<UTT_GOAPAction>> Actions;

	UPROPERTY()
	TArray<UTT_GOAPAction*> CurrentPlan;

	virtual TMap<FName, bool> BuildWorldState() const PURE_VIRTUAL(UTT_GOAPBrainComponent::BuildWorldState, return TMap<FName, bool>(););
	virtual TMap<FName, bool> BuildGoalState() const PURE_VIRTUAL(UTT_GOAPBrainComponent::BuildGoalState, return TMap<FName, bool>(););
	virtual void BuildActions() PURE_VIRTUAL(UTT_GOAPBrainComponent::BuildActions, );

	virtual bool ShouldReplan() const;
	virtual void RebuildPlan();
};