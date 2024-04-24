// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "STUCharacterMovementComponent.generated.h"


class ASTUBaseCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USTUCharacterMovementComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.2", ClampMax = "5.0"))
	float RunModifier = 1.5f;
	
	virtual float GetMaxSpeed() const override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ASTUBaseCharacter* Character = nullptr;
};
