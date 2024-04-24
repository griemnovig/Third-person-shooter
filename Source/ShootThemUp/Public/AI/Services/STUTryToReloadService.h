// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "STUTryToReloadService.generated.h"


UCLASS()
class SHOOTTHEMUP_API USTUTryToReloadService : public UBTService
{
	GENERATED_BODY()
	
public:
	USTUTryToReloadService();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	double Probability = 0.7f;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
