// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_Missile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ATT_Missile::ATT_Missile()
{
	PrimaryActorTick.bCanEverTick = true;

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	RootComponent = MissileMesh;	

	MissileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	MissileMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);	

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

}

void ATT_Missile::BeginPlay()
{
	Super::BeginPlay();

	MissileMesh->OnComponentHit.AddDynamic(this, &ATT_Missile::OnHit);

}

void ATT_Missile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Damage using HealthComponent on OtherActor

	AActor* mOwner = GetOwner();	
	if (!mOwner) return;

	if(!OtherActor || OtherActor == mOwner || OtherActor == this) return;

	UGameplayStatics::ApplyDamage(OtherActor, Damage, mOwner->GetInstigatorController(), this, nullptr);

	//Sound

	Destroy();
}



void ATT_Missile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

