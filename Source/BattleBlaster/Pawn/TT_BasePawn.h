// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TT_BasePawn.generated.h"

class UCapsuleComponent;
class UArrowComponent;
class ATT_Missile;	
class UTT_HealthComponent;	

UCLASS()
class BATTLEBLASTER_API ATT_BasePawn : public APawn
{
	GENERATED_BODY()

public:
	ATT_BasePawn();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(EditAnywhere, Category = "Pawn")
	TObjectPtr<UStaticMeshComponent> BaseMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Pawn")
	TObjectPtr<UStaticMeshComponent> TurretMeshComponent;

	UPROPERTY(EditAnywhere, Category = "VALUE")
	TObjectPtr<UArrowComponent> ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<ATT_Missile> MissileClass;

	UPROPERTY(EditAnywhere, Category = "Pawn")
	TObjectPtr<UTT_HealthComponent> HealthComponent;	

	UFUNCTION()
	virtual void HandleDestruction();	


	UFUNCTION()
	virtual void RotateBase(float Value);

	UFUNCTION()
	virtual void MoveForward(float Value);

	UFUNCTION()
	virtual FVector GetPawnForward() const;

protected:
	virtual void BeginPlay() override;



private:	


};
