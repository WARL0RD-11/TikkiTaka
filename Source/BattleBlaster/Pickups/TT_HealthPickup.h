// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_HealthPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class BATTLEBLASTER_API ATT_HealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	ATT_HealthPickup();
	virtual void Tick(float DeltaTime) override;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	float HealAmount = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	bool bIsActive = true;

	UFUNCTION()
	void OnPickupOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintCallable)
	bool IsPickupActive() const { return bIsActive; }

	UFUNCTION(BlueprintCallable)
	void ConsumePickup();

protected:
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

private:	


};
