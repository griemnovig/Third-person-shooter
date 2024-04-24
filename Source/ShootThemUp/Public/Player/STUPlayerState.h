// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "STUPlayerState.generated.h"

UCLASS()
class SHOOTTHEMUP_API ASTUPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void SetTeamID(const int32 ID) { TeamID = ID; }
	void SetTeamColor(const FLinearColor& Color) { TeamColor = Color; }
	
	void SubtractKill() { --KillsNum; }
	void AddKill() { ++KillsNum; }
	void AddDeath() { ++DeathsNum; }

	UFUNCTION(BlueprintCallable)
	int32 GetTeamID() const { return TeamID; }
	
	UFUNCTION(BlueprintCallable)
	FLinearColor GetTeamColor() const { return TeamColor; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetKillsNum() const { return KillsNum; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetDeathsNum() const { return DeathsNum; }

private:
	FLinearColor TeamColor;
	int32 TeamID;
	int32 KillsNum = 0;
	int32 DeathsNum = 0;
};
