// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TT_HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEBLASTER_API UTT_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTT_HealthComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Health|Parameters")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Health|Parameters")
	float CurrentHealth = 0.f;

	UFUNCTION()
	void OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
		AController* EventInstigator, AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;

private:	
		
};
