// Shoot Them Up Game, All Rights Reserved.

#include "Components/STUAIWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"

void USTUAIWeaponComponent::StartFire()
{
	if(!CurrentWeapon || !bCanFire) return;

	CurrentWeapon->IsAmmoEmpty() ? USTUAIWeaponComponent::NextWeapon() : CurrentWeapon->StartFire();
}

void USTUAIWeaponComponent::NextWeapon()
{
	if (Weapons.Num() < 2 || !bCanEquip) return;

	int32 NextIndex = (CurrentWeaponIndex + 1) % Weapons.Num();

	while (NextIndex != CurrentWeaponIndex)
	{
		if(!Weapons[NextIndex]->IsAmmoEmpty()) break;
		
		NextIndex = (NextIndex + 1) % Weapons.Num();
	}
	if(CurrentWeaponIndex != NextIndex)
	{
		CurrentWeaponIndex = NextIndex;
		EquipWeapon();
	}
}
