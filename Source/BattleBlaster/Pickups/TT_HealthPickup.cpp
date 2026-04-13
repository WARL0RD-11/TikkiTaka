

#include "TT_HealthPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BattleBlaster/Pawn/TT_BasePawn.h"
#include "BattleBlaster/Components/TT_HealthComponent.h"

ATT_HealthPickup::ATT_HealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetSphereRadius(100.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATT_HealthPickup::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATT_HealthPickup::OnPickupOverlap);
}

void ATT_HealthPickup::OnPickupOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	//DEBUG WHAT IS OVERLAPPING

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Overlapped Actor: %s"), *OtherActor->GetName()));

	if (!bIsActive || !OtherActor)
	{
		return;
	}

	ATT_BasePawn* BasePawn = Cast<ATT_BasePawn>(OtherActor);
	if (!BasePawn || !BasePawn->HealthComponent)
	{
		return;
	}

	BasePawn->HealthComponent->Heal(HealAmount);
	ConsumePickup();
}

void ATT_HealthPickup::ConsumePickup()
{
	bIsActive = false;
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

void ATT_HealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.f, 30 * DeltaTime, 0.f));
}