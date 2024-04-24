// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STURespawnComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTTHEMUP_API USTURespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USTURespawnComponent();

	void Respawn(const int32 RespawnTime);
	
	UFUNCTION(BlueprintCallable)
	int32 GetRespawnCountdown() const { return RespawnCountdown; }

	UFUNCTION(BlueprintCallable)
	bool IsRespawnInProgress() const;

private:
	FTimerHandle RespawnTimerHandle;
	int32 RespawnCountdown = 0;

	void RespawnTimerUpdate();
};
