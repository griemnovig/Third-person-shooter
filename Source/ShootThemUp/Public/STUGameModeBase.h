// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "STUCoreTypes.h"
#include "STUGameModeBase.generated.h"

class AAIController;
class APlayerStart;

UCLASS()
class SHOOTTHEMUP_API ASTUGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASTUGameModeBase();

	UPROPERTY(BlueprintAssignable)
	FOnMatchStateChangedSignature OnMatchStateChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnRoundChangingSignature OnRoundChanging;
	
	UPROPERTY(BlueprintAssignable)
	FOnKilledSignature OnKilled;
	
	virtual void StartPlay() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate) override;
	
	UFUNCTION(BlueprintCallable)
	virtual bool ClearPause() override;

	UFUNCTION(BlueprintCallable)
	float GetRoundChangeTime() const { return  RoundChangeTime; }

	UFUNCTION(BlueprintCallable)
	float GetTimeDilationForce() const { return  TimeDilationForce; } 
	
	void Killed(const AController* KillerController, const AController* VictimController) const;
	void RespawnRequest(AController* Controller);
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void GetRoundInfo(int32& RoundsNumOut, int32& CurrentRoundOut, int32& RoundCountdownOut) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<AAIController> AIControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<APawn> AIPawnClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FGameData GameData;

private:
	UPROPERTY()
	TArray<AActor*> PlayerStartActors;
	
	ESTUMatchState MatchState = WaitingToStart;
	int32 CurrentRound = 1;
	int32 RoundCountdown = 0;
	float RoundChangeTime = 4.0f;
	float TimeDilationForce = 0.15f;
	FTimerHandle GameRoundTimerHandle;
	FTimerHandle ChangeRoundTimerHandle;
	
	void SpawnBots();
	void StartRound();
	void GameTimerUpdate();
	void ResetPlayers();
	void ResetOnePlayer(AController* Controller);
	void CreateTeamsInfo() const;
	FLinearColor DetermineColorByTeamID(const int32 TeamID) const;
	void SetPlayerColor(const AController* Controller) const;
	void StartRespawn(const AController* Controller) const;
	void GameOver();
	void SetMatchState(const ESTUMatchState State);
	void StopAllFire() const;
	void CollectAllPlayersStartActors();
	AActor* GetBestPlayerStartActor();
	void ShufflePlayerStartActors();
	void ChangingRound();
	void OnChangeRoundTimeOver();
	void DestroySpawnedProjectiles() const;
	bool IsChangingRoundTime() const { return GetWorldTimerManager().IsTimerActive(ChangeRoundTimerHandle); }
};
