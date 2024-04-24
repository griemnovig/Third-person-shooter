// Shoot Them Up Game, All Rights Reserved.

#include "Weapon/STURifleWeapon.h"
#include "AIController.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Player/STUPlayerController.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "GameFramework/Pawn.h"

ASTURifleWeapon::ASTURifleWeapon()
{
	WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("FXComponent");
}

void ASTURifleWeapon::BeginPlay()
{
	Super::BeginPlay();

	check(WeaponFXComponent);
}

void ASTURifleWeapon::StartFire()
{
	bFireInProgress = true;
	GetWorldTimerManager().SetTimer(
		ShotTimerHandle,
		this,
		&ASTURifleWeapon::MakeShot,
		TimeBetweenShot,
		true);
	
	MakeShot();
}

void ASTURifleWeapon::StopFire()
{
	bFireInProgress = false;
	GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void ASTURifleWeapon::Zoom(bool Enabled)
{
	ASTUPlayerController* PlayerController = GetInstigatorController<ASTUPlayerController>();
	if(!PlayerController || !PlayerController->PlayerCameraManager) return;
	
	float RequiredCameraFOV;
	Enabled ? RequiredCameraFOV = FOVInZoom : RequiredCameraFOV = PlayerController->PlayerCameraManager->DefaultFOV;
	
	PlayerController->InterpCameraFOV(RequiredCameraFOV);
}

void ASTURifleWeapon::MakeShot()
{
	Super::MakeShot();
	if(!CanMakeShot()) return;
	
	FVector StartTraceLocation = FVector::ZeroVector;
	FVector EndTraceLocation = FVector::ZeroVector;
	GetTracePoints(StartTraceLocation, EndTraceLocation);
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetInstigator());
	CollisionParams.bReturnPhysicalMaterial = true;
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartTraceLocation, EndTraceLocation, ECC_Visibility, CollisionParams);
	
	//DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Blue, false, 3.0f);
	//if(HitResult.bBlockingHit) DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 32, FColor::Red, false, 3.0f);
	
	if(HitResult.bBlockingHit)
	{
		MakeDamage(HitResult);
    	WeaponFXComponent->PlayImpactParticleFX(HitResult);	
	}
	
	DecreaseAmmo();
	SpawnShootFX();
	PlayCameraShake();
}

void ASTURifleWeapon::GetTracePoints(FVector& StartTraceLocation, FVector& EndTraceLocation) const
{
	const APawn* InstigatorPawn = Cast<APawn>(GetInstigator());
	if(!InstigatorPawn || !InstigatorPawn->Controller) return;

	FVector MuzzleLocation = GetMuzzleWorldLocation();
	StartTraceLocation = MuzzleLocation;
	
	if(!InstigatorPawn->IsPlayerControlled()) // if Instigator is AI
	{
		const AAIController* AIController = Cast<AAIController>(InstigatorPawn->Controller);
		if(AIController && AIController->GetFocusActor())
		{
			const FVector FocusActorLocation = AIController->GetFocusActor()->GetActorLocation();
			const FRotator RotationToFocusActor = MakeSpread(((FocusActorLocation - StartTraceLocation).GetSafeNormal()).Rotation(), false);
			EndTraceLocation = RotationToFocusActor.Vector() * TraceMaxDistance + StartTraceLocation;
			return;
		}
		EndTraceLocation = MakeSpread(InstigatorPawn->GetBaseAimRotation(), false).Vector() * TraceMaxDistance + StartTraceLocation;
		return;
	}
	// if Instigator is Player
	
	FVector ViewLocation;
	FRotator ViewRotation;
	InstigatorPawn->Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	const FVector EndTraceFromCamera = ViewRotation.Vector() * TraceMaxDistance + ViewLocation;
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(
		HitResult, ViewLocation, EndTraceFromCamera, ECC_Visibility, FCollisionQueryParams("", false, GetInstigator())
		);

	EndTraceLocation = EndTraceFromCamera;
	
	if(HitResult.bBlockingHit)
	{
		const FVector DirectionToImpactPoint = (HitResult.ImpactPoint - MuzzleLocation).GetSafeNormal();
		const FVector DirectionForward = ViewRotation.Vector();
		
		if(DirectionForward.Dot(DirectionToImpactPoint) > SharpnessAngleOfShot)
		{
    		EndTraceLocation = DirectionToImpactPoint * HitResult.Distance + MuzzleLocation;
		}
	}
}

void ASTURifleWeapon::MakeDamage(const FHitResult& HitResult)
{
	AActor* DamagedActor = HitResult.GetActor();
	if(!DamagedActor) return;
	
	AController* Controller = GetInstigatorController<AController>();
	if(!Controller) return;

	FPointDamageEvent PointDamageEvent;
	PointDamageEvent.HitInfo = HitResult;
	
	DamagedActor->TakeDamage(Damage, PointDamageEvent, Controller, this);
}

FRotator ASTURifleWeapon::MakeSpread(const FRotator& ViewRotator, const bool& PlayerInstigator) const
{
	if (PlayerInstigator || FMath::IsNearlyEqual(BulletSpread, 0.0f)) return ViewRotator;

	const float RandomSpreadPitch = FMath::FRandRange(-BulletSpread, BulletSpread);;
	const float RandomSpreadYaw = FMath::FRandRange(-BulletSpread, BulletSpread);;
	const float RandomSpreadRoll = FMath::FRandRange(-BulletSpread, BulletSpread);;
	
	FRotator SpreadedRotator = ViewRotator;
	return SpreadedRotator.Add(RandomSpreadPitch, RandomSpreadYaw, RandomSpreadRoll);
}

void ASTURifleWeapon::PlayCameraShake() const
{
	if(!GetInstigator()) return;
	const APlayerController* Controller = Cast<APlayerController>(GetInstigator()->Controller);
	
	if(!Controller || !Controller->PlayerCameraManager || !ShootCameraShake) return;
	
	Controller->PlayerCameraManager->StartCameraShake(ShootCameraShake);
}
