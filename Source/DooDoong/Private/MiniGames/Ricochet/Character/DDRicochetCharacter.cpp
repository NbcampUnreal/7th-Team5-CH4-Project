//DDRicochetCharacter.cpp
#include "MiniGames/Ricochet/Character/DDRicochetCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Common/DDLogManager.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "MiniGames/Ricochet/Actors/DDRicochetProjectile.h"
#include "MiniGames/Ricochet/GameMode/DDRicochetGameMode.h"

ADDRicochetCharacter::ADDRicochetCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	ThrowComp = CreateDefaultSubobject<USceneComponent>(TEXT("ThrowComp"));
	ThrowComp->SetupAttachment(GetMesh());
}

void ADDRicochetCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Ricochet BeginPlay"));

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (IMC_Ricochet)
			{
				Subsystem->AddMappingContext(IMC_Ricochet, 10);
			}
		}
	}
}

bool ADDRicochetCharacter::BIsCanThrow() const
{
	return ProjectileClass != nullptr;
}

void ADDRicochetCharacter::TryThrow()
{
	UE_LOG(LogTemp, Warning, TEXT("[INPUT] TryThrow called"));
	if (!BIsCanThrow())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[INPUT] HandleThrowMontageNotify"));
	PlayThrowMontage();

	if (ThrowMontage == nullptr)
	{
		HandleThrowMontageNotify();
	}
}

bool ADDRicochetCharacter::GetAimPoint(FVector& OutTargetPoint) const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr || GetWorld() == nullptr)
	{
		return false;
	}

	int32 ViewportX = 0;
	int32 ViewportY = 0;
	PlayerController->GetViewportSize(ViewportX, ViewportY);

	const FVector2D ScreenCenter(
		static_cast<float>(ViewportX) * 0.5f,
		static_cast<float>(ViewportY) * 0.5f);

	FVector TraceStart;
	FVector TraceDirection;

	if (!PlayerController->DeprojectScreenPositionToWorld(
		ScreenCenter.X,
		ScreenCenter.Y,
		TraceStart,
		TraceDirection))
	{
		return false;
	}

	const FVector TraceEnd = TraceStart + (TraceDirection * AimTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RicochetAimTrace), false, this);
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams))
	{
		OutTargetPoint = HitResult.ImpactPoint;
		return true;
	}

	OutTargetPoint = TraceEnd;
	return true;
}

FVector ADDRicochetCharacter::GetThrowLocation() const
{
	if (const USkeletalMeshComponent* MeshComponent = GetMesh();
		MeshComponent != nullptr && ProjectileSocketName != NAME_None && MeshComponent->DoesSocketExist(ProjectileSocketName))
	{
		return MeshComponent->GetSocketLocation(ProjectileSocketName);
	}

	if (ThrowComp != nullptr)
	{
		return ThrowComp->GetComponentLocation();
	}

	return GetActorLocation();
}

FTransform ADDRicochetCharacter::GetProjectileSpawnTransform() const
{
	if (const USkeletalMeshComponent* MeshComponent = GetMesh();
		MeshComponent != nullptr && ProjectileSocketName != NAME_None && MeshComponent->DoesSocketExist(ProjectileSocketName))
	{
		return MeshComponent->GetSocketTransform(ProjectileSocketName, RTS_World);
	}

	if (ThrowComp != nullptr)
	{
		return ThrowComp->GetComponentTransform();
	}

	return GetActorTransform();
}



void ADDRicochetCharacter::HandleThrowMontageNotify()
{
	LOG_JJH(Log, TEXT("[RicochetCharacter] HandleThrowMontageNotify"));

	if (!BIsCanThrow())
	{
		LOG_JJH(Warning, TEXT("[RicochetCharacter] Cannot throw"));
		return;
	}

	FVector TargetPoint;
	if (!GetAimPoint(TargetPoint))
	{
		LOG_JJH(Warning, TEXT("[RicochetCharacter] Failed to get aim point"));
		return;
	}

	LOG_JJH(Log, TEXT("[RicochetCharacter] Request server throw. Location=%s Target=%s"),
		*GetThrowLocation().ToString(),
		*TargetPoint.ToString());

	Server_TryThrow(GetThrowLocation(), TargetPoint);
}

void ADDRicochetCharacter::Server_TryThrow_Implementation(
	const FVector_NetQuantize& ThrowLocation,
	const FVector_NetQuantize& TargetPoint)
{
	LOG_JJH(Log, TEXT("[RicochetCharacter] Server_TryThrow"));

	ADDRicochetGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDRicochetGameMode>() : nullptr;
	if (GameMode == nullptr)
	{
		LOG_JJH(Warning, TEXT("[RicochetCharacter] GameMode is null"));
		return;
	}

	const FVector ThrowDirection = (TargetPoint - ThrowLocation).GetSafeNormal();
	if (ThrowDirection.IsNearlyZero())
	{
		LOG_JJH(Warning, TEXT("[RicochetCharacter] Throw direction is nearly zero"));
		return;
	}

	LOG_JJH(Log, TEXT("[RicochetCharacter] Spawn projectile. Direction=%s"), *ThrowDirection.ToString());
	GameMode->ThrowProjectile(this, ThrowLocation, ThrowDirection.Rotation());
}

void ADDRicochetCharacter::PlayThrowMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] PlayThrowMontage called"));

	if (ThrowMontage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] ThrowMontage is NULL"));
		return;
	}

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
		{
			float Result = AnimInstance->Montage_Play(ThrowMontage);

			UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] Montage_Play result = %f"), Result);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] AnimInstance NULL"));
		}
	}
}




