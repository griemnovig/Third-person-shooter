// Shoot Them Up Game, All Rights Reserved.

#include "Menu/STUMenuGameModeBase.h"
#include "Menu/STUMenuPlayerController.h"

ASTUMenuGameModeBase::ASTUMenuGameModeBase()
{
	PlayerControllerClass = ASTUMenuPlayerController::StaticClass();
}
