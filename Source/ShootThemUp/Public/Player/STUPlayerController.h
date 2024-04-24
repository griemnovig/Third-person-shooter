// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "STUCoreTypes.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "STUPlayerController.generated.h"

class USTURespawnComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASTUPlayerController();
	
	void InterpCameraFOV(float RequiredCameraFOV_In, float FOVInterpTimeDelta_In = 2.0f, float FOVInterpSpeed_In = 2.0f);

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& PlayerName) const { PlayerState->SetPlayerName(PlayerName); }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USTURespawnComponent* RespawnComponent;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	FTimerHandle FOVInterpolationTimerHandle;
	float RequiredCameraFOV = 0.0f;
	float FOVInterpTimeDelta = 0.0f;
	float FOVInterpSpeed = 0.0f;
	float InterpTimerTickRate = 0.02f;

	void OnPauseGame();
	void InterpolateFOV();
	
	UFUNCTION()
	void OnMatchStateChanged(const ESTUMatchState MatchState);
};
