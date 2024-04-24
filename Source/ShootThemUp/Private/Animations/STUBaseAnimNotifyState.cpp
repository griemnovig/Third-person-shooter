// Shoot Them Up Game, All Rights Reserved.


#include "Animations/STUBaseAnimNotifyState.h"


void USTUBaseAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{	
	OnNotifyBegin.Broadcast(MeshComp);
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void USTUBaseAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	OnNotifyEnd.Broadcast(MeshComp);
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
