// Shoot Them Up Game, All Rights Reserved.

#include "Pickups/STUBasePickup.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ASTUBasePickup::ASTUBasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	SetRootComponent(CollisionComponent);
}

void ASTUBasePickup::BeginPlay()
{
	Super::BeginPlay();

	check(CollisionComponent);

	GenerateRotationYaw();
}

bool ASTUBasePickup::GivePickupTo(APawn* PlayerPawn)
{
	return false;
}

void ASTUBasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.0f, RotationYaw, 0.0f));
}

void ASTUBasePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	APawn* Pawn = Cast<APawn>(OtherActor);
	if(!Pawn) return;
	
	if(GivePickupTo(Pawn))
	{
		PickupWasTaken();
		return;
	}
	if (TryToPickupTimerHandle.IsValid()) return;
	GetWorldTimerManager().SetTimer(TryToPickupTimerHandle, this, &ASTUBasePickup::TryToPickupOnTimer, TryToPickupTimerTickRate, true);
}

void ASTUBasePickup::PickupWasTaken()
{
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetRootComponent()->SetVisibility(false, true);
	
	bCouldBeTaken = false;
	
	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASTUBasePickup::Respawn, RespawnTime);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), OnTakenSound, GetActorLocation());
}

void ASTUBasePickup::Respawn()
{
	bCouldBeTaken = true;
	
	GetRootComponent()->SetVisibility(true, true);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	GenerateRotationYaw();
}

void ASTUBasePickup::GenerateRotationYaw()
{
	const float Direction = FMath::RandBool() ? 1.0f : -1.0f;
	RotationYaw = FMath::RandRange(1.0f, 2.0f) * Direction;
}

void ASTUBasePickup::TryToPickupOnTimer()
{
	TArray<AActor*> OverlappedActors;
	GetOverlappingActors(OverlappedActors, APawn::StaticClass());
	if (OverlappedActors.IsEmpty())
	{
		GetWorldTimerManager().ClearTimer(TryToPickupTimerHandle);
		return;
	}
	for (AActor* OverlappedActor : OverlappedActors)
	{
		APawn* OverlappedPawn = Cast<APawn>(OverlappedActor);
		if(OverlappedPawn)
		{
			if (GivePickupTo(OverlappedPawn))
			{
				GetWorldTimerManager().ClearTimer(TryToPickupTimerHandle);
				PickupWasTaken();
				break;
			}
		}
	}
}
