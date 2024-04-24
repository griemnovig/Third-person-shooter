// Shoot Them Up Game, All Rights Reserved.


#include "Pickups/STUHealthPickup.h"
#include "Components/STUHealthComponent.h"


ASTUHealthPickup::ASTUHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool ASTUHealthPickup::GivePickupTo(APawn* PlayerPawn)
{
	USTUHealthComponent* HealthComponent = PlayerPawn->GetComponentByClass<USTUHealthComponent>();
	if (!HealthComponent) return false;

	return HealthComponent->TryToAddHealth(HealthAmount);
}
