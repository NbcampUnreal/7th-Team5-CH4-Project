#include "MiniGames/Shooter/Character/DDShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "MiniGames/Shooter/GameMode/DDShooterGameMode.h"

ADDShooterCharacter::ADDShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
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

bool ADDShooterCharacter::CanFire() const
{
	return true;
}

void ADDShooterCharacter::TryFire()
{
	if (!CanFire())
	{
		return;
	}

	FVector TargetPoint;
	if (!GetCrosshairAimPoint(TargetPoint))
	{
		return;
	}

	Server_TryFire(GetMuzzleLocation(), TargetPoint);
}

bool ADDShooterCharacter::GetCrosshairAimPoint(FVector& OutTargetPoint) const
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
	if (MuzzleComp != nullptr)
	{
		return MuzzleComp->GetComponentLocation();
	}

	return GetActorLocation();
}

void ADDShooterCharacter::Server_TryFire_Implementation(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& TargetPoint)
{
	ADDShooterGameMode* ShooterGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDShooterGameMode>() : nullptr;
	if (ShooterGameMode == nullptr)
	{
		return;
	}

	const FVector ShotDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
	if (ShotDirection.IsNearlyZero())
	{
		return;
	}

	const FRotator ShotRotation = ShotDirection.Rotation();

	// Projectile가 준비되면 GameMode의 발사 함수로 연결
	// ShooterGameMode->FireProjectile(this, MuzzleLocation, ShotRotation);
}
