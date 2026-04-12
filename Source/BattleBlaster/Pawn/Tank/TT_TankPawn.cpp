// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_TankPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATT_TankPawn::ATT_TankPawn()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ATT_TankPawn::BeginPlay()
{
	Super::BeginPlay();
}


void ATT_TankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATT_TankPawn::HandleDestruction()
{
	Super::HandleDestruction();
}

