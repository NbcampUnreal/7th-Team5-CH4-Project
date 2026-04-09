#include "MiniGames/Shooter/Character/DDShooterCharacter.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Common/DDLogManager.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "MiniGames/Shooter/Actors/DDShotProjectile.h"
#include "MiniGames/Shooter/GameMode/DDShooterGameMode.h"

ADDShooterCharacter::ADDShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->TargetArmLength = 200.f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	MuzzleComp = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleComp"));
	MuzzleComp->SetupAttachment(GetMesh());
}

void ADDShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool ADDShooterCharacter::BIsCanFire() const
{
	return ProjectileClass != nullptr;
}

void ADDShooterCharacter::TryFire()
{
	if (!BIsCanFire())
	{
		return;
	}

	PlayFireMontage();

	if (FireMontage == nullptr)
	{
		HandleFireMontageNotify();
	}
}

bool ADDShooterCharacter::GetAimPoint(FVector& OutTargetPoint) const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr || GetWorld() == nullptr)
	{
		return false;
	}

	int32 ViewportX = 0;
	int32 ViewportY = 0;
	PlayerController->GetViewportSize(ViewportX, ViewportY);

	const FVector2D ScreenCenter(static_cast<float>(ViewportX) * 0.5f, static_cast<float>(ViewportY) * 0.5f);

	FVector TraceStart;
	FVector TraceDirection;
	if (!PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, TraceStart, TraceDirection))
	{
		return false;
	}

	const FVector TraceEnd = TraceStart + (TraceDirection * AimTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ShooterAimTrace), false, this);
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		OutTargetPoint = HitResult.ImpactPoint;
		return true;
	}

	OutTargetPoint = TraceEnd;
	return true;
}

FVector ADDShooterCharacter::GetMuzzleLocation() const
{
	if (const USkeletalMeshComponent* MeshComponent = GetMesh();
		MeshComponent != nullptr && ProjectileSocketName != NAME_None && MeshComponent->DoesSocketExist(ProjectileSocketName))
	{
		return MeshComponent->GetSocketLocation(ProjectileSocketName);
	}

	if (MuzzleComp != nullptr)
	{
		return MuzzleComp->GetComponentLocation();
	}

	return GetActorLocation();
}

FTransform ADDShooterCharacter::GetProjectileSpawnTransform() const
{
	if (const USkeletalMeshComponent* MeshComponent = GetMesh();
		MeshComponent != nullptr && ProjectileSocketName != NAME_None && MeshComponent->DoesSocketExist(ProjectileSocketName))
	{
		return MeshComponent->GetSocketTransform(ProjectileSocketName, RTS_World);
	}

	if (MuzzleComp != nullptr)
	{
		return MuzzleComp->GetComponentTransform();
	}

	return GetActorTransform();
}

void ADDShooterCharacter::HandleFireMontageNotify()
{
	LOG_JJH(Log, TEXT("[ShooterCharacter] HandleFireMontageNotify"));

	if (!BIsCanFire())
	{
		LOG_JJH(Warning, TEXT("[ShooterCharacter] Cannot fire"));
		return;
	}

	FVector TargetPoint;
	if (!GetAimPoint(TargetPoint))
	{
		LOG_JJH(Warning, TEXT("[ShooterCharacter] Failed to get aim point"));
		return;
	}

	LOG_JJH(Log, TEXT("[ShooterCharacter] Request server fire. Muzzle=%s Target=%s"),
	        *GetMuzzleLocation().ToString(),
	        *TargetPoint.ToString());
	Server_TryFire(GetMuzzleLocation(), TargetPoint);
}

void ADDShooterCharacter::Server_TryFire_Implementation(const FVector_NetQuantize& MuzzleLocation,
	const FVector_NetQuantize& TargetPoint)
{
	LOG_JJH(Log, TEXT("[ShooterCharacter] Server_TryFire"));

	ADDShooterGameMode* ShooterGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDShooterGameMode>() : nullptr;
	if (ShooterGameMode == nullptr)
	{
		LOG_JJH(Warning, TEXT("[ShooterCharacter] ShooterGameMode is null"));
		return;
	}

	const FVector ShotDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
	if (ShotDirection.IsNearlyZero())
	{
		LOG_JJH(Warning, TEXT("[ShooterCharacter] Shot direction is nearly zero"));
		return;
	}

	LOG_JJH(Log, TEXT("[ShooterCharacter] Spawn projectile. Direction=%s"), *ShotDirection.ToString());
	ShooterGameMode->FireProjectile(this, MuzzleLocation, ShotDirection.Rotation());
}

void ADDShooterCharacter::PlayFireMontage()
{
	if (FireMontage == nullptr)
	{
		return;
	}

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
		{
			AnimInstance->Montage_Play(FireMontage);
		}
	}
}
