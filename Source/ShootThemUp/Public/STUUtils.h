#pragma once

#include "Player/STUPlayerState.h"

class STUUtils
{
public:
	bool static AreEnemies(const AController* FirstController, const AController* SecondController)
	{
		if(!FirstController || !SecondController || FirstController == SecondController) return false;

		const ASTUPlayerState* FirstPlayerState = Cast<ASTUPlayerState>(FirstController->PlayerState);
		const ASTUPlayerState* SecondPlayerState = Cast<ASTUPlayerState>(SecondController->PlayerState);
		
		return FirstPlayerState && SecondPlayerState && FirstPlayerState->GetTeamID() != SecondPlayerState->GetTeamID();
	}

	bool static ThereIsEnemieInArray(const AController* ReferenceController, const TArray<AActor*>& OtherActors)
	{
		if(!ReferenceController || OtherActors.IsEmpty()) return false;
		
		for(const AActor* CompareActor : OtherActors)
		{
			const APawn* ComparePawn = Cast<APawn>(CompareActor);
			if(ComparePawn && AreEnemies(ReferenceController, ComparePawn->Controller)) return true;
		}
		return false;
	}
};
