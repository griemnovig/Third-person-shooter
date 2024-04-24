// Shoot Them Up Game, All Rights Reserved.

#include "STUGameModeBase.h"
#include "AIController.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerController.h"
#include "Player/STUPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/STURespawnComponent.h"
#include "EngineUtils.h"
#include "STUUtils.h"
#include "Components/STUWeaponComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Weapon/STUProjectile.h"


ASTUGameModeBase::ASTUGameModeBase()
{
	DefaultPawnClass = ASTUBaseCharacter::StaticClass();
	PlayerControllerClass = ASTUPlayerController::StaticClass();
	PlayerStateClass = ASTUPlayerState::StaticClass();
}

void ASTUGameModeBase::StartPlay()
{
	Super::StartPlay();
	
	CollectAllPlayersStartActors();
	
	SpawnBots();
	CreateTeamsInfo();
	
	CurrentRound = 1;
	StartRound();

	SetMatchState(InProgress);
}

UClass* ASTUGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if(InController && InController->IsA<AAIController>())
	{
		return AIPawnClass;
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

bool ASTUGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	const bool bPauseSet = Super::SetPause(PC, CanUnpauseDelegate);
	
	if(bPauseSet)
	{
		StopAllFire();
		SetMatchState(Pause);
	}
	return bPauseSet;
}

bool ASTUGameModeBase::ClearPause()
{
	const bool bPauseClear = Super::ClearPause();
	if(bPauseClear)
	{
		SetMatchState(InProgress);
	}
	return bPauseClear;
}

void ASTUGameModeBase::Killed(const AController* KillerController, const AController* VictimController) const
{
	if(IsChangingRoundTime()) return;
	
	ASTUPlayerState* KillerPlayerState = KillerController ? Cast<ASTUPlayerState>(KillerController->PlayerState) : nullptr;
	ASTUPlayerState* VictimPlayerState = VictimController ? Cast<ASTUPlayerState>(VictimController->PlayerState) : nullptr;
	
	if(KillerPlayerState)
	{
		STUUtils::AreEnemies(KillerController, VictimController) ? KillerPlayerState->AddKill() : KillerPlayerState->SubtractKill();
	}

	if(VictimPlayerState)
	{
		VictimPlayerState->AddDeath();
	}
	
	OnKilled.Broadcast(KillerController, VictimController);
	StartRespawn(VictimController);
}

void ASTUGameModeBase::GetRoundInfo(int32& RoundsNumOut, int32& CurrentRoundOut, int32& RoundCountdownOut) const
{
	RoundsNumOut = GameData.RoundsNum;
	CurrentRoundOut = CurrentRound;
	RoundCountdownOut = RoundCountdown;
}

void ASTUGameModeBase::RespawnRequest(AController* Controller)
{
	ResetOnePlayer(Controller);
}

void ASTUGameModeBase::SpawnBots()
{
	if(!GetWorld()) return;

	for(int32 i = 0; i < GameData.PlayersNum - 1; ++i)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AAIController* STUAIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnInfo);
		RestartPlayer(STUAIController);
	}
}

void ASTUGameModeBase::StartRound()
{
	RoundCountdown = GameData.RoundTime;
	GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &ASTUGameModeBase::GameTimerUpdate, 1.0f, true);
}

void ASTUGameModeBase::GameTimerUpdate()
{
	if(--RoundCountdown <= 0)
	{
		GetWorldTimerManager().ClearTimer(GameRoundTimerHandle);

		if(CurrentRound + 1 <= GameData.RoundsNum)
		{
			++CurrentRound;
			ChangingRound();
		}
		else
		{
			GameOver();
		}
	}
}

void ASTUGameModeBase::ResetPlayers()
{
	if(!GetWorld()) return;

	for(auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ResetOnePlayer(It->Get());
	}
}

void ASTUGameModeBase::ResetOnePlayer(AController* Controller)
{
	if(Controller && Controller->GetPawn())
	{
		Controller->GetPawn()->Reset();
	}
	if(PlayerStartActors.IsEmpty())
	{
		RestartPlayer(Controller);
		SetPlayerColor(Controller);
		return;
	}
	
	RestartPlayerAtPlayerStart(Controller, GetBestPlayerStartActor());
	SetPlayerColor(Controller);
}

void ASTUGameModeBase::CreateTeamsInfo() const
{
	if(!GetWorld()) return;

	int32 TeamID = 1;
	int32 ControllerCounter = 0;
	
	for(auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		const AController* Controller = It->Get();
		if(!Controller) continue;

		ASTUPlayerState* PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
		if(!PlayerState) continue;

		PlayerState->SetTeamID(TeamID);
		PlayerState->SetTeamColor(DetermineColorByTeamID(TeamID));
		if(!Controller->IsPlayerController())
		{
			PlayerState->SetPlayerName("Bot " + FString::FromInt(++ControllerCounter));
		}
		SetPlayerColor(Controller);
		
		TeamID = TeamID == 1 ? 2 : 1;
	}
}

FLinearColor ASTUGameModeBase::DetermineColorByTeamID(const int32 TeamID) const
{
	if(TeamID - 1 < GameData.TeamColors.Num())
	{
		return  GameData.TeamColors[TeamID - 1];
	}
	return GameData.DefaultTeamColor;
}

void ASTUGameModeBase::SetPlayerColor(const AController* Controller) const
{
	if(!Controller) return;

	const ASTUBaseCharacter* Character = Cast<ASTUBaseCharacter>(Controller->GetPawn());
	if(!Character) return;

	const ASTUPlayerState* PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
	if(!PlayerState) return;

	Character->SetPlayerColor(PlayerState->GetTeamColor());
}

void ASTUGameModeBase::StartRespawn(const AController* Controller) const
{
	if(!Controller) return;

	const bool RespawnAvailable = RoundCountdown > GameData.MinRoundTimeForRespawn + GameData.RespawnTime;
	if(!RespawnAvailable) return;
	
	USTURespawnComponent* RespawnComponent = Controller->FindComponentByClass<USTURespawnComponent>();
	if(!RespawnComponent) return;
	
	RespawnComponent->Respawn(GameData.RespawnTime);
}

void ASTUGameModeBase::GameOver()
{
	for(APawn* Pawn : TActorRange<APawn>(GetWorld()))
	{
		if(Pawn)
		{
			Pawn->TurnOff();
			Pawn->DisableInput(nullptr);
		}
	}
	SetMatchState(ESTUMatchState::GameOver);
}

void ASTUGameModeBase::SetMatchState(const ESTUMatchState State)
{
	if(MatchState == State) return;

	MatchState = State;
	OnMatchStateChanged.Broadcast(MatchState);
}

void ASTUGameModeBase::StopAllFire() const
{
	for(const APawn* Pawn : TActorRange<APawn>(GetWorld()))
	{
		if(Pawn)
		{
			USTUWeaponComponent* WeaponComponent = Pawn->FindComponentByClass<USTUWeaponComponent>();
			if(!WeaponComponent) continue;

			WeaponComponent->StopFire();
			WeaponComponent->Zoom(false);
		}
	}
}

void ASTUGameModeBase::CollectAllPlayersStartActors()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);
}

AActor* ASTUGameModeBase::GetBestPlayerStartActor()
{
	ShufflePlayerStartActors();
	
	for(AActor* PlayerStartActor : PlayerStartActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(PlayerStartActor);
		if(PlayerStart->PlayerStartTag == "Block") continue;
		return PlayerStart;
	}
	AActor* RandomPlayerStart = PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num()-1)];
	return RandomPlayerStart;
}

void ASTUGameModeBase::ShufflePlayerStartActors()
{
	const int32 LastIndex = PlayerStartActors.Num() - 1;
	for (int32 i = 0; i <= LastIndex; ++i)
	{
		const int32 RandIndex = FMath::RandRange(i, LastIndex);
		if (i != RandIndex)
		{
			PlayerStartActors.Swap(i, RandIndex);
		}
	}
}

void ASTUGameModeBase::ChangingRound()
{
	if(GetWorld())
	{
		OnRoundChanging.Broadcast(CurrentRound);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilationForce);
		const float ChangeTimeUnderDilation = RoundChangeTime * TimeDilationForce;
		
		GetWorldTimerManager().SetTimer(
			ChangeRoundTimerHandle,
			this,
			&ASTUGameModeBase::OnChangeRoundTimeOver,
			ChangeTimeUnderDilation,
			false);
	}
}

void ASTUGameModeBase::OnChangeRoundTimeOver()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	DestroySpawnedProjectiles();
	ResetPlayers();
	StartRound();
}

void ASTUGameModeBase::DestroySpawnedProjectiles() const
{
	if(!GetWorld()) return;
	
	TArray<AActor*> SpawnedProjectiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTUProjectile::StaticClass(), SpawnedProjectiles);
	for(AActor* SpawnedProjectile : SpawnedProjectiles)
	{
		if(!SpawnedProjectile) continue;
		SpawnedProjectile->Destroy();
	}
}
