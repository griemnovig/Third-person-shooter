// Shoot Them Up Game, All Rights Reserved.

#include "AI/Services/STUChangeWeaponService.h"
#include "Components/STUAIWeaponComponent.h"
#include "AIController.h"

USTUChangeWeaponService::USTUChangeWeaponService()
{
	NodeName = "Change Weapon";
}

void USTUChangeWeaponService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if(FMath::FRand() <= Probability && !FMath::IsNearlyEqual(Probability, 0.0f))
	{
		const AAIController* Controller = OwnerComp.GetAIOwner();
		if(Controller)
		{
			USTUAIWeaponComponent* WeaponComponent = Controller->GetPawn()->FindComponentByClass<USTUAIWeaponComponent>();
			if(WeaponComponent)
			{
				WeaponComponent->NextWeapon();
			}
		}
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}








