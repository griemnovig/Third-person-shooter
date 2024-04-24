// Shoot Them Up Game, All Rights Reserved.

#include "Weapon/Components/STUWeaponFXComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundCue.h"

USTUWeaponFXComponent::USTUWeaponFXComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponFXComponent::PlayImpactParticleFX(const FHitResult& Hit)
{
	FImpactData ImpactData = DefaultImpactData;

	if(Hit.PhysMaterial.IsValid())
	{
		const UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
		if(ImpactDataMap.Contains(PhysMat))
		{
			ImpactData = ImpactDataMap[PhysMat];
		}
	}
	// spawn particle system
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
		ImpactData.ParticleSystem,
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation());
	
	// spawn decal
	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
		ImpactData.DecalData.Material,
		ImpactData.DecalData.Size,
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation());

	if(DecalComponent)
	{
		DecalComponent->SetFadeOut(ImpactData.DecalData.LifeTime, ImpactData.DecalData.FadeOutTime);
		DecalComponent->SetFadeScreenSize(0.0f);
	}

	// play impact sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),
		ImpactData.ImpactSound,
		Hit.ImpactPoint);
}
