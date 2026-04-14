#include "BattleBlaster/AI/GOAP/TT_GOAPAction.h"

bool UTT_GOAPAction::CheckProceduralPrecondition(UTT_GOAPBrainComponent* Brain) const
{
	return true;
}

bool UTT_GOAPAction::Perform(UTT_GOAPBrainComponent* Brain, float DeltaTime)
{
	return true;
}

bool UTT_GOAPAction::IsDone(UTT_GOAPBrainComponent* Brain) const
{
	return true;
}

void UTT_GOAPAction::ResetAction(UTT_GOAPBrainComponent* Brain)
{
}