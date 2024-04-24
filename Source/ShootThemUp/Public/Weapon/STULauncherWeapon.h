// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "STUBaseWeapon.h"
#include "STULauncherWeapon.generated.h"

class ASTUProjectile;

UCLASS(Abstract)
class SHOOTTHEMUP_API ASTULauncherWeapon : public ASTUBaseWeapon
{
	GENERATED_BODY()

public:
	virtual void StartFire() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ASTUProjectile>  ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName RearMuzzleSocketName = "MuzzleFlashRear";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* RearMuzzleFlash;

	float SharpnessAngleOfShot = 0.85f;
	virtual void MakeShot() override;
	virtual void SpawnShootFX() const override;
};
