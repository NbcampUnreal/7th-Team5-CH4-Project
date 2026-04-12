#include "MiniGames/Shooter/Actors/DDShooterTargetSpawner.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "MiniGames/Shooter/Actors/DDShooterTarget.h"

ADDShooterTargetSpawner::ADDShooterTargetSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	LaunchDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchDirectionArrow"));
	LaunchDirectionArrow->SetupAttachment(RootComponent);
	LaunchDirectionArrow->ArrowSize = 1.5f;

	TargetClass = ADDShooterTarget::StaticClass();
}

ADDShooterTarget* ADDShooterTargetSpawner::SpawnTarget()
{
	if (!HasAuthority() || !bEnabled || TargetClass == nullptr || GetWorld() == nullptr)
	{
		return nullptr;
	}

	const FVector LaunchDirection = GetLaunchDirection();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADDShooterTarget* Target = GetWorld()->SpawnActor<ADDShooterTarget>(
		TargetClass,
		GetActorLocation(),
		LaunchDirection.Rotation(),
		SpawnParams);

	if (Target != nullptr)
	{
		Target->LaunchTarget(LaunchDirection, GetRandomSpeed());
	}

	return Target;
}

FVector ADDShooterTargetSpawner::GetLaunchDirection() const
{
	FRotator LaunchRotation = GetActorRotation();

	LaunchRotation.Yaw += FMath::FRandRange(-RandomYawAngle, RandomYawAngle);
	LaunchRotation.Pitch += FMath::FRandRange(-RandomPitchAngle, RandomPitchAngle);

	return LaunchRotation.Vector().GetSafeNormal();
}

float ADDShooterTargetSpawner::GetRandomSpeed() const
{
	const float LowerSpeed = FMath::Min(MinSpeed, MaxSpeed);
	const float UpperSpeed = FMath::Max(MinSpeed, MaxSpeed);

	return FMath::FRandRange(LowerSpeed, UpperSpeed);
}
