// Shoot Them Up Game, All Rights Reserved.

#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ASTUBaseWeapon::ASTUBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
}

void ASTUBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	check(WeaponMesh);
	checkf(DefaultAmmoData.Bullets > 0, TEXT("Bullets count couldn't be <= 0"));
	checkf(DefaultAmmoData.Clips >= 0, TEXT("Clips count couldn't be < 0"));
	CurrentAmmo = DefaultAmmoData;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const
{
	return WeaponMesh->GetSocketTransform(MuzzleSocketName).GetLocation();
}

void ASTUBaseWeapon::DecreaseAmmo()
{
	if (CurrentAmmo.Bullets == 0) return;
	
	CurrentAmmo.Bullets--;

	if (IsClipEmpty() && !IsAmmoEmpty())
	{
		StopFire();
		OnClipEmpty.Broadcast();
	}
}

void ASTUBaseWeapon::SpawnShootFX() const
{
	UGameplayStatics::SpawnSoundAttached(StartFireSound, WeaponMesh, MuzzleSocketName);
	
	UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlash,
		WeaponMesh,
		MuzzleSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true);
}

bool ASTUBaseWeapon::TryToAddAmmo(const int32 ClipAmount)
{
	if (CurrentAmmo.bInfiniteAmmo || IsAmmoFull() || ClipAmount <= 0) return false;

	if(CurrentAmmo.Clips < DefaultAmmoData.Clips)
	{
		CurrentAmmo.Clips = FMath::Clamp(CurrentAmmo.Clips + ClipAmount, 0, DefaultAmmoData.Clips);
		return true;
	}
	return false;
}

void ASTUBaseWeapon::ChangeClip()
{
	if (!CurrentAmmo.bInfiniteAmmo)
	{
		if (CurrentAmmo.Clips == 0) return;
		CurrentAmmo.Clips--;
	}
	CurrentAmmo.Bullets = DefaultAmmoData.Bullets;
}

bool ASTUBaseWeapon::CanReload() const
{
	return CurrentAmmo.Bullets < DefaultAmmoData.Bullets && CurrentAmmo.Clips > 0;
}

bool ASTUBaseWeapon::CanMakeShot()
{
	if (!IsClipEmpty()) return true;
	
	StopFire();
	UGameplayStatics::SpawnSoundAttached(NoAmmoSound, WeaponMesh);
	return false;
}
