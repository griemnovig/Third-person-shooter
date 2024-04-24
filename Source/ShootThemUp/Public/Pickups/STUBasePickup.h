// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBasePickup.generated.h"

class USphereComponent;
class USoundCue;

UCLASS(Abstract)
class SHOOTTHEMUP_API ASTUBasePickup : public AActor
{
	GENERATED_BODY()

public:
	ASTUBasePickup();
	virtual void Tick(float DeltaTime) override;
	bool CouldBeTaken() const { return bCouldBeTaken; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float RespawnTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* OnTakenSound;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void BeginPlay() override;
	void PickupWasTaken();

private:
	float RotationYaw = 0.0f;
	float TryToPickupTimerTickRate = 0.5f;
	FTimerHandle TryToPickupTimerHandle;
	bool bCouldBeTaken = true;
	
	virtual bool GivePickupTo(APawn* PlayerPawn);
	void TryToPickupOnTimer();
	void Respawn();
	void GenerateRotationYaw();
};
