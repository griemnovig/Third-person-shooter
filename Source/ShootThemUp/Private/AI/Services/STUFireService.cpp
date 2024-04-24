// Shoot Them Up Game, All Rights Reserved.


#include "AI/Services/STUFireService.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/STUAIWeaponComponent.h"

USTUFireService::USTUFireService()
{
	NodeName = TEXT("Fire");
}

void USTUFireService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const AActor* OwnerActor = OwnerComp.GetAIOwner()->GetPawn();
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	
	const AActor* EnemyActor = Cast<AActor>(Blackboard->GetValueAsObject(EnemyActorKey.SelectedKeyName));
	const bool bHasAim = Blackboard && EnemyActor && OwnerActor && IsRotatedOnTarget(OwnerActor, EnemyActor);
	
	if(OwnerActor)
	{
		USTUAIWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USTUAIWeaponComponent>();
		if(WeaponComponent)
		{
			bHasAim ? WeaponComponent->StartFire() : WeaponComponent->StopFire();
		}
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

bool USTUFireService::IsRotatedOnTarget(const AActor* OwnerActor,const AActor* TargetActor) const
{
	if(!TargetActor || !OwnerActor) return false;
	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	const FVector OwnerForwardVector = OwnerActor->GetActorForwardVector();
	
	const float AngleToTarget = FVector::DotProduct(OwnerForwardVector, (TargetLocation - OwnerLocation).GetSafeNormal());
	if(AngleToTarget >= AcceptableAngleRotationToStartFire) return true;
	return false;
}
