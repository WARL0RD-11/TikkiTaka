// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_Missile.h"
#include "GameFramework/ProjectileMovementComponent.h"

ATT_Missile::ATT_Missile()
{
	PrimaryActorTick.bCanEverTick = true;

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	RootComponent = MissileMesh;	

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

}

void ATT_Missile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATT_Missile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

