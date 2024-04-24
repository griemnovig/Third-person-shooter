// Shoot Them Up Game, All Rights Reserved.

#include "AI/Services/STUTryToReloadService.h"
#include "Components/STUAIWeaponComponent.h"
#include "AIController.h"


USTUTryToReloadService::USTUTryToReloadService()
{
	NodeName = "Reload Weapon";
}

void USTUTryToReloadService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if(FMath::FRand() <= Probability && !FMath::IsNearlyEqual(Probability, 0.0f))
	{
		const AAIController* Controller = OwnerComp.GetAIOwner();
		if(Controller)
		{
			USTUAIWeaponComponent* WeaponComponent = Controller->GetPawn()->FindComponentByClass<USTUAIWeaponComponent>();
			if(WeaponComponent && WeaponComponent->IsCurrentWeaponClipNotFull())
			{
				WeaponComponent->Reload();
			}
		}
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}


