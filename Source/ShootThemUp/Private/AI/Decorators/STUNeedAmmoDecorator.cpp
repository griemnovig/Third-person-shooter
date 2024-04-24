// Shoot Them Up Game, All Rights Reserved.

#include "AI/Decorators/STUNeedAmmoDecorator.h"
#include "AIController.h"
#include "Components/STUAIWeaponComponent.h"

USTUNeedAmmoDecorator::USTUNeedAmmoDecorator()
{
	NodeName = "Need Ammo";
}

bool USTUNeedAmmoDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!Controller) return false;
	
	const USTUAIWeaponComponent* WeaponComponent = Controller->GetPawn()->FindComponentByClass<USTUAIWeaponComponent>();
	if(!WeaponComponent) return false;
	
	return WeaponComponent->NeedAmmo(WeaponType);
}
