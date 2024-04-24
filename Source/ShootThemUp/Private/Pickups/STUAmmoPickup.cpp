// Shoot Them Up Game, All Rights Reserved.


#include "Pickups/STUAmmoPickup.h"
#include "Components/STUWeaponComponent.h"


ASTUAmmoPickup::ASTUAmmoPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool ASTUAmmoPickup::GivePickupTo(APawn* PlayerPawn)
{
	const auto WeaponComponent = PlayerPawn->GetComponentByClass<USTUWeaponComponent>();
	if (!WeaponComponent) return false;

	return WeaponComponent->TryToAddAmmo(WeaponType, ClipsAmount);
}
