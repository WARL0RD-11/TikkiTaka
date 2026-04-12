//My Tank Pawn class

#pragma once

#include "CoreMinimal.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "TT_TankPawn.generated.h"

/**
 * 
 */

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BATTLEBLASTER_API ATT_TankPawn : public ATT_BasePawn
{
	GENERATED_BODY()
	
public:
	ATT_TankPawn();
	virtual void Tick(float DeltaTime) override;

	virtual void HandleDestruction() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
};
