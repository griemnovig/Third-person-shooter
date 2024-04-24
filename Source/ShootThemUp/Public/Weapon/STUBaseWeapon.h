// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUCoreTypes.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;
class USoundCue;

UCLASS(Abstract)
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ASTUBaseWeapon();

	FOnClipEmptySignature OnClipEmpty;
	
	virtual void StartFire() {};
	virtual void StopFire() {};
	virtual void Zoom(bool Enabled) {};
	
	void ChangeClip();
	bool CanReload() const;
	bool IsFiring() const { return bFireInProgress; }
	bool TryToAddAmmo(const int32 ClipAmount);
	bool IsAmmoEmpty() const { return !CurrentAmmo.bInfiniteAmmo && CurrentAmmo.Clips == 0 && IsClipEmpty(); }
	bool IsAmmoFull() const { return CurrentAmmo.Clips == DefaultAmmoData.Clips && CurrentAmmo.Bullets == DefaultAmmoData.Bullets; }
	bool IsClipsFull() const { return !CurrentAmmo.bInfiniteAmmo && CurrentAmmo.Clips == DefaultAmmoData.Clips; }
	bool IsClipNotFull() const { return CurrentAmmo.Bullets < DefaultAmmoData.Bullets; }
	
	FWeaponUIData GetUIData() const { return UIData; };
	FAmmoData GetAmmoData() const { return CurrentAmmo; };
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleSocket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float TraceMaxDistance = 3500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FAmmoData DefaultAmmoData{20, 5, false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FWeaponUIData UIData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* StartFireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* NoAmmoSound;

	float SharpnessAngleOfShot = 0.65f;
	bool bFireInProgress = false;
	
	virtual void BeginPlay() override;
	bool CanMakeShot();
	virtual void MakeShot() {};
	FVector GetMuzzleWorldLocation() const;
	virtual void SpawnShootFX() const;
	void DecreaseAmmo();
	bool IsClipEmpty() const { return CurrentAmmo.Bullets == 0; }
	
private:
	FAmmoData CurrentAmmo;
};
