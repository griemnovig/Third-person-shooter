// Shoot Them Up Game, All Rights Reserved.

#include "Components/STUWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h"
#include "Animations/STUEquipWeaponAnimState.h"
#include "Animations/STUReloadWeaponAnimState.h"
#include "Animations/STUWeaponSwapAnimState.h"
#include "Animations/AnimUtils.h"

constexpr static int32 WeaponNum = 2;

USTUWeaponComponent::USTUWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USTUWeaponComponent::TryToAddAmmo(const TSubclassOf<ASTUBaseWeapon> WeaponType, const int32 ClipAmount)
{
	for (const auto Weapon : Weapons)
	{
		if(Weapon && Weapon->IsA(WeaponType))
		{
			if(Weapon->TryToAddAmmo(ClipAmount))
			{
				if(CurrentWeapon->IsA(WeaponType))
				{
					Reload();
				}
				return true;
			}
		}
	}
	return false;
}

void USTUWeaponComponent::Zoom(const bool Enabled) const
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Zoom(Enabled);
	}
}

bool USTUWeaponComponent::GetWeaponUIData(FWeaponUIData& UIData) const
{
	if(CurrentWeapon)
	{
		UIData = CurrentWeapon->GetUIData();
		return true;
	}
	return false;
}

bool USTUWeaponComponent::GetWeaponAmmoData(FAmmoData& AmmoData) const
{
	if(CurrentWeapon)
	{
		AmmoData = CurrentWeapon->GetAmmoData();
		return true;
	}
	return false;
}

bool USTUWeaponComponent::NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType) const
{
	for(const ASTUBaseWeapon* Weapon : Weapons)
	{
		if(Weapon && Weapon->IsA(WeaponType))
		{
			return !Weapon->IsClipsFull();
		} 
	}
	return false;
}

void USTUWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(WeaponData.Num() == WeaponNum, TEXT("We can hold only %f weapons"), WeaponNum);
	
	InitAnimations();
	SpawnWeapons();
	EquipWeapon();
}

void USTUWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CurrentWeapon = nullptr;
	
	for (ASTUBaseWeapon* Weapon : Weapons)
	{
		if(!Weapon) continue;
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->Destroy();
	}
	Weapons.Empty();

	Super::EndPlay(EndPlayReason);
}

void USTUWeaponComponent::SpawnWeapons()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !GetWorld()) return;

	for (FWeaponData OneWeaponData : WeaponData)
	{
		ASTUBaseWeapon* Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);
		if(!Weapon) continue;
		Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::OnEmptyClip);
		Weapon->SetInstigator(Character);
		Weapons.Add(Weapon);
		AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
	}
}

void USTUWeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
	if (!Weapon || !SceneComponent) return;
	
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void USTUWeaponComponent::EquipWeapon()
{	
	if(CurrentWeapon)
	{
		bCanEquip = false;
		bCanFire = false;
		bCanReload = false;
		CurrentWeapon->StopFire();
		Zoom(false);
		
		PlayAnimationMontage(EquipAnimMontage);
	}
	else
	{
		SwitchingWeapon();  // Setup default weapon on begin without animation and other...
	}
}

void USTUWeaponComponent::StartFire()
{
	bWantToFire = true;
	if(!CurrentWeapon || !bCanFire) return;
	
	CurrentWeapon->StartFire();
}

void USTUWeaponComponent::StopFire()
{
	bWantToFire = false;
	if(!CurrentWeapon) return;
	
	CurrentWeapon->StopFire();
}

void USTUWeaponComponent::NextWeapon()
{		
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	if(!Character || !Character->GetMesh() || Weapons.Num() < 2 || !bCanEquip) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if(AnimInstance && AnimInstance->Montage_IsPlaying(CurrentReloadAnimMontage)) // Trying abort playing Reload Montage.
	{
		USTUReloadWeaponAnimState* ReloadFinishNotify = AnimUtils::FindFirstNotifyByClass<USTUReloadWeaponAnimState>(CurrentReloadAnimMontage);
		if(ReloadFinishNotify)
		{
			ReloadFinishNotify->OnNotifyEnd.RemoveAll(this);
			AnimInstance->Montage_Stop(0.5f, CurrentReloadAnimMontage);
		}
	}
	CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
	EquipWeapon();
}

void USTUWeaponComponent::Reload()
{
	ChangeClip();
}

void USTUWeaponComponent::PlayAnimationMontage(UAnimMontage* Animation) const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;
	
	Character->PlayAnimMontage(Animation);
}

void USTUWeaponComponent::InitAnimations()
{	
	USTUEquipWeaponAnimState* EquipFinishedNotify = AnimUtils::FindFirstNotifyByClass<USTUEquipWeaponAnimState>(EquipAnimMontage);
	if(EquipFinishedNotify)
	{
		EquipFinishedNotify->OnNotifyEnd.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
	}
	else
	{
		checkNoEntry();
	}

	USTUWeaponSwapAnimState* WeaponSwapNotify = AnimUtils::FindFirstNotifyByClass<USTUWeaponSwapAnimState>(EquipAnimMontage);
	if(WeaponSwapNotify)
	{
		WeaponSwapNotify->OnNotifyBegin.AddUObject(this, &USTUWeaponComponent::OnWeaponSwap);
	}
	else
	{
		checkNoEntry();
	}
}

void USTUWeaponComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent)
{
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComponent) return;
	
	bCanEquip = true;
	bCanFire = true;
	bCanReload = true;
	
	if(!CurrentWeapon) return;
	if (CurrentWeapon->GetAmmoData().Bullets == 0) { Reload(); };
	if (bWantToFire) {StartFire();}
}

void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent)
{
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComponent || !CurrentWeapon) return;
	
	CurrentWeapon->ChangeClip();
	bCanEquip = true;
	bCanFire = true;
	bCanReload = true;
	
	if (bWantToFire) {StartFire();}
}

void USTUWeaponComponent::OnWeaponSwap(USkeletalMeshComponent* MeshComponent)
{
	// This is the moment when the character puts his hand back during the weapon switching animation.
	
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComponent) return;
	
	SwitchingWeapon();
}

void USTUWeaponComponent::SwitchingWeapon()
{
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;
	
	if(CurrentWeapon)
	{
		AttachWeaponToSocket(CurrentWeapon, Character->GetMesh() , WeaponArmorySocketName);
	}
	CurrentWeapon = Weapons.IsValidIndex(CurrentWeaponIndex) ? Weapons[CurrentWeaponIndex] : nullptr;
	AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);

	SetReloadMontageForCurrentWeapon();
	OnSwitchWeapon.Broadcast();
}

void USTUWeaponComponent::SetReloadMontageForCurrentWeapon()
{
			// Setup current ReloadMontage for the new current weapon.
	const FWeaponData* CurrentWeaponData = WeaponData.FindByPredicate([&](const FWeaponData& Data){ //
		return Data.WeaponClass == CurrentWeapon->GetClass();});
	CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
 
	if (!CurrentReloadAnimMontage)
	{
		checkNoEntry();
		return;
	}
	
			// Setup new FinishReload anim delegate for the new current weapon.
	USTUReloadWeaponAnimState* ReloadFinishNotify = AnimUtils::FindFirstNotifyByClass<USTUReloadWeaponAnimState>(CurrentReloadAnimMontage);
	if(ReloadFinishNotify)
	{
		if (ReloadFinishNotify->OnNotifyEnd.IsBoundToObject(this)) return;
		ReloadFinishNotify->OnNotifyEnd.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
	}
	else
	{
		checkNoEntry();
	}
}

void USTUWeaponComponent::OnEmptyClip()
{
	ChangeClip();
}

void USTUWeaponComponent::ChangeClip()
{
	if (!bCanReload || !CurrentWeapon || !CurrentWeapon->CanReload()) return;
	
	if (!CurrentReloadAnimMontage)
	{
		CurrentWeapon->StopFire();
		CurrentWeapon->ChangeClip();
		return;
	}
	bCanFire = false;
	bCanReload = false;
	CurrentWeapon->StopFire();
	PlayAnimationMontage(CurrentReloadAnimMontage);
}
