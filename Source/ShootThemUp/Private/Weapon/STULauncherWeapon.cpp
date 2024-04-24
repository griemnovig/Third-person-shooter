// Shoot Them Up Game, All Rights Reserved.

#include "Weapon/STULauncherWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/STUProjectile.h"

void ASTULauncherWeapon::StartFire()
{
	MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
	Super::MakeShot();
	if(!CanMakeShot()) return;
	
	const APawn* InstigatorPawn = Cast<APawn>(GetInstigator());
	if(!InstigatorPawn || !InstigatorPawn->Controller) return;

	const FVector MuzzleLocation = GetMuzzleWorldLocation();
	FVector ViewLocation = MuzzleLocation;
	FRotator ViewRotation;
	
	if(!InstigatorPawn->IsPlayerControlled()) // if Instigator is AI
	{
		ViewRotation = InstigatorPawn->GetBaseAimRotation();
	}
	else
	{
		InstigatorPawn->Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);	
	}
	
	const FVector DirectionForward = ViewRotation.Vector();
	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = DirectionForward * TraceMaxDistance + TraceStart;
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		FCollisionQueryParams("", false, GetInstigator())
		);

	FRotator FinalDirection = ViewRotation;
	
	if(HitResult.bBlockingHit)
	{
		const FVector DirectionToImpactPoint = (HitResult.ImpactPoint - MuzzleLocation).GetSafeNormal();
		if(DirectionForward.Dot(DirectionToImpactPoint) > SharpnessAngleOfShot)
		{
			FinalDirection = DirectionToImpactPoint.Rotation();
		}
	}
	
	const FTransform ProjectileSpawnTransform(FinalDirection, MuzzleLocation);
	ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(
		ProjectileClass,
		ProjectileSpawnTransform,
		this,
		GetInstigator()
		);
	
	if (Projectile)
	{
		Projectile->FinishSpawning(ProjectileSpawnTransform);
		DecreaseAmmo();
		SpawnShootFX();
	}
}

void ASTULauncherWeapon::SpawnShootFX() const
{
	Super::SpawnShootFX();
	
	if(!RearMuzzleFlash) return;
	
	UGameplayStatics::SpawnEmitterAttached(
		RearMuzzleFlash,
		WeaponMesh,
		RearMuzzleSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
		);
}


