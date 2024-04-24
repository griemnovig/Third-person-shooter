// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "STUBaseAnimNotifyState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotifyBeginSignature, USkeletalMeshComponent*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotifyEndSignature, USkeletalMeshComponent*);

UCLASS(Abstract)
class SHOOTTHEMUP_API USTUBaseAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	UFUNCTION()
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	
	FOnNotifyBeginSignature OnNotifyBegin;
	FOnNotifyEndSignature OnNotifyEnd;
};
