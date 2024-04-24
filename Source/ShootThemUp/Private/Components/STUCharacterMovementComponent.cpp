// Shoot Them Up Game, All Rights Reserved.


#include "Components/STUCharacterMovementComponent.h"
#include "Player/STUBaseCharacter.h"


USTUCharacterMovementComponent::USTUCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTUCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<ASTUBaseCharacter>(GetPawnOwner());
}

float USTUCharacterMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	
	return Character && Character->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}
