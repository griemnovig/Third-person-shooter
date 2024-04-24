// Shoot Them Up Game, All Rights Reserved.

#include "AI/Services/STUFindEnemyService.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/STUAIPerceptionComponent.h"

USTUFindEnemyService::USTUFindEnemyService()
{
	NodeName = TEXT("Find Enemy");
}

void USTUFindEnemyService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if(Blackboard)
	{
		AAIController* Controller = OwnerComp.GetAIOwner();
		const USTUAIPerceptionComponent* PerceptionComponent = Controller->FindComponentByClass<USTUAIPerceptionComponent>();
		if(PerceptionComponent)
		{
			AActor* ClosestEnemy = PerceptionComponent->GetClosestEnemy();
			Blackboard->SetValueAsObject(EnemyActorKey.SelectedKeyName, ClosestEnemy);
			if(ClosestEnemy)
			{
				Controller->SetFocus(ClosestEnemy);
				Blackboard->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, ClosestEnemy->GetActorLocation());
			}
			else
			{
				Controller->SetFocus(nullptr);
			}
		}
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
