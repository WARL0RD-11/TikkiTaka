#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TT_GOAPAction.generated.h"

class UTT_GOAPBrainComponent;

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class BATTLEBLASTER_API UTT_GOAPAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GOAP")
	FName ActionName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GOAP")
	float Cost = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GOAP")
	TMap<FName, bool> Preconditions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GOAP")
	TMap<FName, bool> Effects;

	virtual bool CheckProceduralPrecondition(UTT_GOAPBrainComponent* Brain) const;
	virtual bool Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime);
	virtual bool IsDone(UTT_GOAPBrainComponent* Brain) const;
	virtual void ResetAction(UTT_GOAPBrainComponent* Brain);
};