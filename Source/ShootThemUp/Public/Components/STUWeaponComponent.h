// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUCoreTypes.h"
#include "Weapon/STUBaseWeapon.h"
#include "STUWeaponComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTUWeaponComponent();

	UPROPERTY(BlueprintAssignable)
	FOnSwitchWeaponSignature OnSwitchWeapon;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "UI")
	bool GetWeaponUIData(FWeaponUIData& UIData) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "UI")
	bool GetWeaponAmmoData(FAmmoData& AmmoData) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=true, Category = "UI")
	bool IsCurrentWeaponNoAmmo() const { return CurrentWeapon && CurrentWeapon->IsAmmoEmpty() ;}
	
	bool NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType) const;
	bool IsCurrentWeaponClipNotFull() const { return CurrentWeapon && CurrentWeapon->IsClipNotFull(); }
	virtual void StartFire();
	void StopFire();
	virtual void NextWeapon();
	void Reload();
	bool IsFiring() const { return CurrentWeapon && CurrentWeapon->IsFiring(); }
	bool TryToAddAmmo(const TSubclassOf<ASTUBaseWeapon> WeaponType, const int32 ClipAmount);
	void Zoom(const bool Enabled) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<FWeaponData> WeaponData;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponEquipSocketName = "WeaponSocket";

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponArmorySocketName = "ArmorySocket";

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* EquipAnimMontage = nullptr;

	UPROPERTY()
	ASTUBaseWeapon* CurrentWeapon = nullptr;

	UPROPERTY()
	TArray<ASTUBaseWeapon*> Weapons;

	int32 CurrentWeaponIndex = 0;
	bool bCanEquip = true;
	bool bCanFire = true;

	void EquipWeapon();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

 private:
	UPROPERTY()
	UAnimMontage* CurrentReloadAnimMontage = nullptr;
	
	bool bCanReload = true;
	bool bWantToFire = false;
	
	virtual void SpawnWeapons();
	void PlayAnimationMontage(UAnimMontage* Animation) const;
	void InitAnimations();
	void OnEquipFinished(USkeletalMeshComponent* MeshComponent);
	void OnReloadFinished(USkeletalMeshComponent* MeshComponent);
	void OnWeaponSwap(USkeletalMeshComponent* MeshComponent);
	void SwitchingWeapon();
	void SetReloadMontageForCurrentWeapon();
	void OnEmptyClip();
	void ChangeClip();
};
