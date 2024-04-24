// Shoot Them Up Game, All Rights Reserved.

#include "AI/Decorators/STUHealthPercentDecorator.h"
#include "AIController.h"
#include "Components/STUHealthComponent.h"

USTUHealthPercentDecorator::USTUHealthPercentDecorator()
{
	NodeName = "Health Percent";
}

bool USTUHealthPercentDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!Controller) return false;
	
	const USTUHealthComponent* HealthComponent = Controller->GetPawn()->FindComponentByClass<USTUHealthComponent>();
	if(!HealthComponent || HealthComponent->IsDead()) return false;

	return HealthComponent->GetHealthPercent() <= HealthPercent;
}
