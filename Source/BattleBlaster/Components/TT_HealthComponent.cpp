// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_HealthComponent.h"
#include "BattleBlaster/GameMode/TikkiTakaGameMode.h"

UTT_HealthComponent::UTT_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}




void UTT_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UTT_HealthComponent::OnDamageTaken);

}


void UTT_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTT_HealthComponent::OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* EventInstigator, AActor* DamageCauser)
{
	//Apply damage
	if (Damage <= 0.f) return;

	CurrentHealth -= Damage;
	if(CurrentHealth <= 0.f)
	{
		//Handle death
		if (ATikkiTakaGameMode* GM = Cast<ATikkiTakaGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->ActorDied(DamagedActor);
		}
	}
}

void UTT_HealthComponent::Heal(float HealValue)
{ 
	if (HealValue <= 0.f) return;

	CurrentHealth = FMath::Clamp(CurrentHealth + HealValue, 0.f, MaxHealth);
}

float UTT_HealthComponent::GetHealthPercent() const
{
	if (MaxHealth <= 0.f) return 0.f;

	return (CurrentHealth / MaxHealth) * 100.f;
}