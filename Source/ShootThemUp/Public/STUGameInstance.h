// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "STUCoreTypes.h"
#include "STUGameInstance.generated.h"

class USoundClass;

UCLASS()
class SHOOTTHEMUP_API USTUGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FLevelData GetStartupGameLevel() const { return StartupData; }
	
	UFUNCTION(BlueprintCallable)
	void SetStartupGameLevel(const FLevelData& LevelData) { StartupData = LevelData; }

	UFUNCTION(BlueprintCallable)
	TArray<FLevelData> GetLevelsData() const { return LevelsData;}

	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UWorld> GetMainMenuLevel() const { return MainMenuLevel; }

	UFUNCTION(BlueprintCallable)
	void ToggleVolume();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TArray<FLevelData> LevelsData;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundClass* MasterSoundClass;

private:
	FLevelData StartupData;
};
