// Shoot Them Up Game, All Rights Reserved.

#include "Player/STUPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/CapsuleComponent.h"

ASTUPlayerCharacter::ASTUPlayerCharacter(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	CameraCollisionComponent = CreateDefaultSubobject<USphereComponent>("CameraCollisionComponent");
	CameraCollisionComponent->SetupAttachment(CameraComponent);
	CameraCollisionComponent->SetSphereRadius(115.0f);
	CameraCollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void ASTUPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	check (CameraCollisionComponent);
	
	CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionBeginOverlap);
	CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionEndOverlap);
}

void ASTUPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	check(WeaponComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ASTUPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASTUPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASTUPlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnAround", this, &ASTUPlayerCharacter::AddControllerYawInput);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUPlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUPlayerCharacter::OnStartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUPlayerCharacter::OnStopRunning);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASTUPlayerCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASTUPlayerCharacter::OnStopFire);
	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::NextWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Reload);
	
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASTUPlayerCharacter::ZoomOn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASTUPlayerCharacter::ZoomOff);
}

void ASTUPlayerCharacter::MoveForward(const float Amount)
{
	bIsMovingForward = Amount > 0.0f;
	AddMovementInput(GetActorForwardVector(), Amount);
	if(IsRunning())
	{
		if(WeaponComponent->IsFiring())
		{
			OnStopFire();
		}
		GetWorldTimerManager().ClearTimer(DelayFireTimerHandle);
		ZoomOff();
	}
}

void ASTUPlayerCharacter::MoveRight(const float Amount)
{
	AddMovementInput(GetActorRightVector(), Amount);
}

void ASTUPlayerCharacter::OnStartRunning()
{
	bWantsToRun = true;
	if (IsRunning())
	{
		WeaponComponent->StopFire();
		ZoomOff();
	}
}

void ASTUPlayerCharacter::OnStopRunning()
{
	bWantsToRun = false;
}

bool ASTUPlayerCharacter::IsRunning() const
{
	if (bWantsToRun && bIsMovingForward && !GetVelocity().IsZero())
	{
		return true;
	}
	return false;
}

void ASTUPlayerCharacter::AutoStartFireAfterSprinting() const
{
	if(!WeaponComponent || IsRunning()) return;
	WeaponComponent->StartFire();
}

void ASTUPlayerCharacter::OnStartFire()
{
	if (IsRunning())
	{
		bWantsToRun = false;
		
		GetWorldTimerManager().SetTimer(
			DelayFireTimerHandle,
			this,
			&ASTUPlayerCharacter::AutoStartFireAfterSprinting,
			DelayBeforeFire,
			false);
		
		return;
	}
	WeaponComponent->StartFire();
}

void ASTUPlayerCharacter::OnStopFire()
{
	GetWorldTimerManager().ClearTimer(DelayFireTimerHandle);
	WeaponComponent->StopFire();
}

void ASTUPlayerCharacter::ZoomOn()
{
	if (IsRunning())
	{
		return;
	}
	WeaponComponent->Zoom(true);
}

void ASTUPlayerCharacter::ZoomOff()
{
	WeaponComponent->Zoom(false);
}

void ASTUPlayerCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                        bool bFromSweep, const FHitResult& SweepResult)
{
	CheckCameraOverlap();
}

void ASTUPlayerCharacter::OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckCameraOverlap();
}

void ASTUPlayerCharacter::CheckCameraOverlap() const
{
	const bool bHideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
	GetMesh()->SetOwnerNoSee(bHideMesh);
	
	TArray<USceneComponent*> MeshChildrenComponents;
	GetMesh()->GetChildrenComponents(true, MeshChildrenComponents);
	
	for(USceneComponent* MeshChildrenComponent : MeshChildrenComponents)
	{
		if(MeshChildrenComponent)
		{
			MeshChildrenComponent->SetVisibility(!bHideMesh, true);
		}
	}
}

void ASTUPlayerCharacter::OnDeath()
{
	Super::OnDeath();
	ZoomOff();
	GetWorldTimerManager().ClearTimer(DelayFireTimerHandle);
	
	if(Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}
