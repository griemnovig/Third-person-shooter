// Shoot Them Up Game, All Rights Reserved.


#include "Weapon/STUProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon/Components/STUWeaponFXComponent.h"


ASTUProjectile::ASTUProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->InitSphereRadius(4.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->bReturnMaterialOnMove = true;
	SetRootComponent(CollisionComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 2000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.1f;

	WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("FXComponent");
}

void ASTUProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	check(ProjectileMovementComponent)
	check(CollisionComponent)
	check(WeaponFXComponent)
	
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
	CollisionComponent->OnComponentHit.AddDynamic(this, &ASTUProjectile::OnProjectileHit);
	SetLifeSpan(LifeSeconds);
}

void ASTUProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!GetWorld()) return;

	ProjectileMovementComponent->StopMovementImmediately();
	
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		DamageAmount,
		GetActorLocation(),
		RadiusOfDamage,
		UDamageType::StaticClass(),
		{},
		this,
		GetInstigator()->Controller,
		bDoFullDamage,
		ECC_Vehicle
		);

	// DrawDebugSphere(GetWorld(), GetActorLocation(), RadiusOfDamage, 24, FColor::Red, false, 3.0f);
	WeaponFXComponent->PlayImpactParticleFX(Hit);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.ImpactPoint);
	Destroy();
}

void ASTUProjectile::Destroyed()
{
	Super::Destroyed();
}
