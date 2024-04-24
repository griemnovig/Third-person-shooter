// Shoot Them Up Game, All Rights Reserved.

#include "AI/STUAIController.h"
#include "AI/STUAICharacter.h"
#include "Components/STUAIPerceptionComponent.h"
#include "Components/STURespawnComponent.h"

ASTUAIController::ASTUAIController()
{
	RespawnComponent = CreateDefaultSubobject<USTURespawnComponent>("RespawnComponent");
	
	AIPerceptionComponent = CreateDefaultSubobject<USTUAIPerceptionComponent>(TEXT("STUAIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	bWantsPlayerState = true;
}

void ASTUAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const ASTUAICharacter* AICharacter = Cast<ASTUAICharacter>(InPawn);
	if(AICharacter)
	{
		RunBehaviorTree(AICharacter->BehaviorTreeAsset);
	}
}
