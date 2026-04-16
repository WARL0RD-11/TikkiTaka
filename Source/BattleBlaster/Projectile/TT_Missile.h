// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "TT_Missile.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;

UCLASS()
class BATTLEBLASTER_API ATT_Missile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATT_Missile();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Missile|Mesh")
	TObjectPtr<UStaticMeshComponent> MissileMesh;

	UPROPERTY(EditAnywhere, Category = "Missile|Movement")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;


	UPROPERTY(EditAnywhere, Category = "Missile|Movement")
	TObjectPtr<UNiagaraComponent> TrailParticle;

	UPROPERTY(EditAnywhere, Category = "Missile|Movement")
	TObjectPtr<UNiagaraSystem> HitParticles;

	UPROPERTY(EditAnywhere, Category = "Missile|CameraShake")	
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass;


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Missile|Damage")
	float Damage = 25.f;

protected:
	virtual void BeginPlay() override;	

};
