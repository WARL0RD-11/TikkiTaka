// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "BattleBlaster/Components/TT_HealthComponent.h"

ATT_BasePawn::ATT_BasePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));	
	SetRootComponent(CapsuleComponent);

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh Component"));
	BaseMeshComponent->SetupAttachment(CapsuleComponent);

	TurretMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh Component"));
	TurretMeshComponent->SetupAttachment(BaseMeshComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Projectile Spawn Point"));	
	ProjectileSpawnPoint->SetupAttachment(TurretMeshComponent);

	HealthComponent = CreateDefaultSubobject<UTT_HealthComponent>(TEXT("Health Component"));

}

void ATT_BasePawn::HandleDestruction()
{
	if(DeathParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());
	}

	if(DeathCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
	}
}

void ATT_BasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATT_BasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATT_BasePawn::RotateBase(float Value)
{
	AddActorLocalRotation(FRotator(0.f, Value, 0.f));
}

void ATT_BasePawn::MoveForward(float Value)
{
	AddActorLocalOffset(FVector(Value, 0.f, 0.f), true);
}

FVector ATT_BasePawn::GetPawnForward() const
{
	return GetActorForwardVector();
}