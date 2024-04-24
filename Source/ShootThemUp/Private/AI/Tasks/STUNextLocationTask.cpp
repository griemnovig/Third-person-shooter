// Shoot Them Up Game, All Rights Reserved.

#include "AI/Tasks/STUNextLocationTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

USTUNextLocationTask::USTUNextLocationTask()
{
	NodeName = "Next Location";
}

EBTNodeResult::Type USTUNextLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!GetWorld() || !Controller || !Blackboard) return EBTNodeResult::Failed;
	
	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	const APawn* Pawn = Controller->GetPawn();
	if(!Pawn || !NavSys) return EBTNodeResult::Failed;

	FNavLocation NavLocation;
	FVector Location = Pawn->GetActorLocation();
	if(!SelfCenter)
	{
		const AActor* CenterActor = Cast<AActor>(Blackboard->GetValueAsObject(CenterActorKey.SelectedKeyName));
		if(!CenterActor) return EBTNodeResult::Failed;
		
		Location = CenterActor->GetActorLocation();
	}
	
	const bool Found = NavSys->GetRandomReachablePointInRadius(Location, Radius, NavLocation);
	if(!Found) return EBTNodeResult::Failed;
	
	Blackboard->SetValueAsVector(AimLocationKey.SelectedKeyName, NavLocation.Location);
	return EBTNodeResult::Succeeded;
}
