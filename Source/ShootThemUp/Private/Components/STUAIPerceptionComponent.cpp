// Shoot Them Up Game, All Rights Reserved.


#include "Components/STUAIPerceptionComponent.h"
#include "Components/STUHealthComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"
#include "STUUtils.h"

AActor* USTUAIPerceptionComponent::GetClosestEnemy() const
{
	const AController* OwnerController = Cast<AController>(GetOwner());
	if(!OwnerController) return nullptr;
	
	TArray<AActor*> PercieveActors;
	GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PercieveActors);
	
	if(PercieveActors.IsEmpty() || !STUUtils::ThereIsEnemieInArray(OwnerController, PercieveActors))
	{
		PercieveActors.Empty();
		
		GetKnownPerceivedActors(UAISense_Damage::StaticClass(), PercieveActors);
		if(PercieveActors.IsEmpty() || !STUUtils::ThereIsEnemieInArray(OwnerController, PercieveActors)) return nullptr;
	}

	const APawn* OwnerPawn = OwnerController->GetPawn();
	if(!OwnerPawn) return nullptr;

	float BestDistance = MAX_FLT;
	AActor* ClosestPawn = nullptr;
	for (AActor* PercieveActor : PercieveActors)
	{
		const USTUHealthComponent* HealthComponent = PercieveActor->FindComponentByClass<USTUHealthComponent>();
		
		const APawn* PercievePawn = Cast<APawn>(PercieveActor);
		const bool AreEnemies = PercievePawn && STUUtils::AreEnemies(OwnerController, PercievePawn->Controller);
		
		if (HealthComponent && !HealthComponent->IsDead() && AreEnemies)
		{
			const float CurrentDistance = (PercieveActor->GetActorLocation() - OwnerPawn->GetActorLocation()).Size();
			if(CurrentDistance < BestDistance)
			{
				BestDistance = CurrentDistance;
				ClosestPawn = PercieveActor;
			}
		}
	}
	return ClosestPawn;
}
