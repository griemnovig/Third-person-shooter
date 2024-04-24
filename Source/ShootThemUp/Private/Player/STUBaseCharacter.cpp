// Shoot Them Up Game, All Rights Reserved.

#include "Player/STUBaseCharacter.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/DamageEvents.h"
#include "Components/STUWeaponComponent.h"


ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit):Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	STUHealthComponent = CreateDefaultSubobject<USTUHealthComponent>("STUHealthComponent");
	WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>("STUWeaponComponent");
}

void ASTUBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(STUHealthComponent);
	check(GetCharacterMovement());
	check(GetMesh());
	
	STUHealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
	STUHealthComponent->OnHealthChanged.AddDynamic(this, &ASTUBaseCharacter::OnHealthChanged);
	OnHealthChanged(STUHealthComponent->GetHealth());

	LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);
}

bool ASTUBaseCharacter::IsRunning() const
{
	return false;
}

void ASTUBaseCharacter::SetPlayerColor(const FLinearColor& Color) const
{
	UMaterialInstanceDynamic* MaterialInst = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	if(!MaterialInst) return;
	
	MaterialInst->SetVectorParameterValue(MaterialColorName, Color);
}

void ASTUBaseCharacter::TurnOff()
{
	StopAnyAction();
	
	Super::TurnOff();
}

void ASTUBaseCharacter::Reset()
{
	StopAnyAction();
	
	Super::Reset();
}

void ASTUBaseCharacter::StopAnyAction() const
{
	const AAIController* AIController = Cast<AAIController>(Controller);
	if(AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->Cleanup();
	}
	
	WeaponComponent->StopFire();
	WeaponComponent->Zoom(false);
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit)
{
	const float FallVelocityZ = -GetVelocity().Z;
	if (FallVelocityZ < LandedDamageVelocity.X) return;

	const float FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity, LandedDamage, FallVelocityZ);
	TakeDamage(FinalDamage, FPointDamageEvent(), nullptr, nullptr);
}

void ASTUBaseCharacter::OnDeath()
{
	StopAnimMontage(GetCurrentMontage());
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(LifeSpanOnDeath);
	
	WeaponComponent->StopFire();
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
}
