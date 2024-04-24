// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "STUBaseWeapon.h"
#include "STURifleWeapon.generated.h"

class USTUWeaponFXComponent;

UCLASS(Abstract)
class SHOOTTHEMUP_API ASTURifleWeapon : public ASTUBaseWeapon
{
	GENERATED_BODY()

public:
	ASTURifleWeapon();
	
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void Zoom(bool Enabled) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "VFX")
	USTUWeaponFXComponent* WeaponFXComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "Damage")
	float Damage = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float BulletSpread = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float TimeBetweenShot = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FOV")
	float FOVInZoom = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TSubclassOf<UCameraShakeBase> ShootCameraShake;
	
	float SharpnessAngleOfShot = 0.65f;
	
	virtual void BeginPlay() override;
	virtual void MakeShot() override;
	virtual void MakeDamage(const FHitResult& HitResult);
	
private:
	FTimerHandle ShotTimerHandle;
	FRotator MakeSpread(const FRotator& ViewRotator, const bool& PlayerInstigator) const;
	void GetTracePoints(FVector& StartTraceLocation, FVector& EndTraceLocation) const;
	void PlayCameraShake() const;
};
