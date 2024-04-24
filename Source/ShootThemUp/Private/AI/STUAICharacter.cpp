// Shoot Them Up Game, All Rights Reserved.

#include "AI/STUAICharacter.h"
#include "AI/STUAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/STUAIWeaponComponent.h"
#include "BrainComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ASTUAICharacter::ASTUAICharacter(const FObjectInitializer& ObjInit):Super(ObjInit.SetDefaultSubobjectClass<USTUAIWeaponComponent>("STUWeaponComponent"))
{
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = ASTUAIController::StaticClass();

	bUseControllerRotationYaw = false;
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 285.0f, 0.0f);
	}
	
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("HealthWidgetComponent");
	HealthWidgetComponent->SetupAttachment(GetRootComponent());
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthWidgetComponent->SetDrawAtDesiredSize(true);
	HealthWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASTUAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RespawnSound, GetActorLocation());
	check(HealthWidgetComponent);
}

void ASTUAICharacter::OnDeath()
{
	Super::OnDeath();
	AAIController* STUAIController = GetController<AAIController>();
	if(STUAIController && STUAIController->BrainComponent)
	{
		STUAIController->BrainComponent->Cleanup();
		STUAIController->UnPossess();
	}
}
