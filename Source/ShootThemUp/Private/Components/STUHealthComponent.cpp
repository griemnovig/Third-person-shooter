// Shoot Them Up Game, All Rights Reserved.

#include "Components/STUHealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Camera/CameraShakeBase.h"
#include "Perception/AISense_Damage.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "STUGameModeBase.h"


USTUHealthComponent::USTUHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USTUHealthComponent::TryToAddHealth(float HealthAmount)
{
	if (IsHealthFull() || IsDead()) return false;
	
	SetHealth(Health + HealthAmount);
	return true;
}

void USTUHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(MaxHealth > 0, TEXT("MaxHealth can't be <= 0"));
	SetHealth(MaxHealth);
	
	AActor* OwnerActor = GetOwner();
	if(OwnerActor)
	{
		OwnerActor->OnTakePointDamage.AddDynamic(this, &USTUHealthComponent::OnTakePointDamage);
		OwnerActor->OnTakeRadialDamage.AddDynamic(this, &USTUHealthComponent::OnTakeRadialDamage);
	}
}

void USTUHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
	const UDamageType* DamageType, AActor* DamageCauser)
{
	const float FinalDamage = Damage * GetPointDamageModifier(DamagedActor, BoneName);
	ApplyDamage(FinalDamage, InstigatedBy);
}

void USTUHealthComponent::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	ApplyDamage(Damage, InstigatedBy);
}

void USTUHealthComponent::ApplyDamage(const float Damage, const AController* InstigatedBy)
{
	if (Damage <= 0.0f || IsDead() || bGodMode) return;
	
	SetHealth(Health - Damage);
	GetWorld()->GetTimerManager().ClearTimer(AutoHealTimerHandle);
	
	if(InstigatedBy && InstigatedBy->GetPawn() && GetOwner())
	{
		UAISense_Damage::ReportDamageEvent(
			GetWorld(),
			GetOwner(),
			InstigatedBy->GetPawn(),
			Damage,
			InstigatedBy->GetPawn()->GetActorLocation(),
			GetOwner()->GetActorLocation());
	}
	
	if(IsDead())
	{
		Killed(InstigatedBy);
		OnDeath.Broadcast();
	}
	else if (bAutoHeal)
	{
		GetWorld()->GetTimerManager().SetTimer(
			AutoHealTimerHandle,
			this,
			&USTUHealthComponent::AutoHealing,
			HealUpdateTime,
			true,
			HealDelay);
		
		PlayCameraShake();
	}
}

float USTUHealthComponent::GetPointDamageModifier(AActor* DamagedActor, const FName& BoneName)
{
	const ACharacter* DamagedCharacter = Cast<ACharacter>(DamagedActor);
	if(!DamagedCharacter || !DamagedCharacter->GetMesh() || !DamagedCharacter->GetMesh()->GetBodyInstance(BoneName)) return 1.0f;
	
	const UPhysicalMaterial* PhysMaterial = DamagedCharacter->GetMesh()->GetBodyInstance(BoneName)->GetSimplePhysicalMaterial();
	if(!DamageModifiers.Contains(PhysMaterial)) return 1.0f;

	return DamageModifiers[PhysMaterial];
}

void USTUHealthComponent::PlayCameraShake() const
{
	const APlayerController* Controller = Cast<APawn>(GetOwner())->GetController<APlayerController>();
	if(!Controller || !Controller->PlayerCameraManager) return;
	
	Controller->PlayerCameraManager->StartCameraShake(CameraShake);
}

void USTUHealthComponent::Killed(const AController* KillerController) const
{
	if(!GetWorld()) return;
	const ASTUGameModeBase* GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
	if(!GameMode) return;

	const APawn* Player = Cast<APawn>(GetOwner());
	const AController* VictimController = Player ? Player->Controller : nullptr;

	GameMode->Killed(KillerController, VictimController);
}

void USTUHealthComponent::AutoHealing()
{
	if (IsHealthFull() || IsDead())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoHealTimerHandle);
		return;
	}
	SetHealth(Health + HealModifire);
}

void USTUHealthComponent::SetHealth(const float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);
}
