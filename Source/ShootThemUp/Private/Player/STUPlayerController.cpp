// Shoot Them Up Game, All Rights Reserved.

#include "Player/STUPlayerController.h"
#include "Components/STURespawnComponent.h"
#include "STUCoreTypes.h"
#include "STUGameModeBase.h"

ASTUPlayerController::ASTUPlayerController()
{
	RespawnComponent = CreateDefaultSubobject<USTURespawnComponent>("RespawnComponent");
}

void ASTUPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(GetWorld())
	{
		ASTUGameModeBase* GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
		if(GameMode)
		{
			GameMode->OnMatchStateChanged.AddUniqueDynamic(this, &ASTUPlayerController::OnMatchStateChanged);
		}
	}
}

void ASTUPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if(!InputComponent) return;

	InputComponent->BindAction("PauseGame", IE_Pressed, this, &ASTUPlayerController::OnPauseGame);
}

void ASTUPlayerController::OnPauseGame()
{
	if(!GetWorld()) return;
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if(!GameMode) return;
	
	GameMode->SetPause(this);
}

void ASTUPlayerController::OnMatchStateChanged(const ESTUMatchState MatchState)
{
	if(MatchState == InProgress)
	{
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
		
		int32 SizeX = 0;
		int32 SizeY = 0;
		GetViewportSize(SizeX, SizeY);
		SetMouseLocation(SizeX/2, SizeY/2);
	}
}

void ASTUPlayerController::InterpCameraFOV(float RequiredCameraFOV_In, float FOVInterpTimeDelta_In, float FOVInterpSpeed_In)
{
	RequiredCameraFOV = RequiredCameraFOV_In;
	FOVInterpTimeDelta = FOVInterpTimeDelta_In;
	FOVInterpSpeed = FOVInterpSpeed_In;
	
	if(!PlayerCameraManager)
	{
		GetWorldTimerManager().ClearTimer(FOVInterpolationTimerHandle);
		return;
	}
	
	if(GetWorldTimerManager().IsTimerActive(FOVInterpolationTimerHandle)) return;
	
	GetWorldTimerManager().SetTimer(
		FOVInterpolationTimerHandle,
		this,
		&ASTUPlayerController::InterpolateFOV,
		InterpTimerTickRate,
		true);
}

void ASTUPlayerController::InterpolateFOV()
{
	const float CurrentFOV = PlayerCameraManager->GetFOVAngle();
	const bool bDesiredFOVReached = FMath::IsNearlyEqual(CurrentFOV, RequiredCameraFOV);
	
	if(bDesiredFOVReached)
	{
		GetWorldTimerManager().ClearTimer(FOVInterpolationTimerHandle);
		return;
	}
	
	const float InterpolatedFOV = FMath::FInterpConstantTo(CurrentFOV, RequiredCameraFOV, FOVInterpTimeDelta, FOVInterpSpeed);
	PlayerCameraManager->SetFOV(InterpolatedFOV);
}
