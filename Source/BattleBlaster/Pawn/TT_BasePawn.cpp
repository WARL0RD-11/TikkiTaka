// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"

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

}

void ATT_BasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATT_BasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
