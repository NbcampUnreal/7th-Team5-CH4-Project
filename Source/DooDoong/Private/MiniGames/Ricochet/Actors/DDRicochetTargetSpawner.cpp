//DDRicochetTargetSpawner.cpp
#include "MiniGames/Ricochet/Actors/DDRicochetTargetSpawner.h"
#include "MiniGames/Ricochet/GameState/DDRicochetGameState.h"
#include "GameFramework/GameStateBase.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "MiniGames/Ricochet/Actors/DDRicochetTarget.h"
#include "MiniGames/Ricochet/GameMode/DDRicochetGameMode.h"

ADDRicochetTargetSpawner::ADDRicochetTargetSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ThrowDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ThrowDirectionArrow"));
	ThrowDirectionArrow->SetupAttachment(RootComponent);
	ThrowDirectionArrow->ArrowSize = 1.5f;

	TargetClass = ADDRicochetTarget::StaticClass();
}



void ADDRicochetTargetSpawner::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[Spawner] BeginPlay: %s NetMode=%d"),
		*GetName(),
		GetWorld()->GetNetMode());

	if (UWorld* World = GetWorld())
	{
		if (ADDRicochetGameState* GS = World->GetGameState<ADDRicochetGameState>())
		{
			GS->RegisterSpawner(this);
		}
	}
}

void ADDRicochetTargetSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (ADDRicochetGameState* GS = World->GetGameState<ADDRicochetGameState>())
		{
			GS->UnregisterSpawner(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}




ADDRicochetTarget* ADDRicochetTargetSpawner::SpawnTarget()
{
	if (!HasAuthority() || !bEnabled || TargetClass == nullptr || GetWorld() == nullptr)
	{
		return nullptr;
	}

	const FVector ThrowDirection = GetThrowDirection();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADDRicochetTarget* Target = GetWorld()->SpawnActor<ADDRicochetTarget>(
		TargetClass,
		GetActorLocation(),
		ThrowDirection.Rotation(),
		SpawnParams);

	if (Target != nullptr)
	{
		Target->ThrowTarget(ThrowDirection, GetRandomSpeed());
	}

	return Target;
}

FVector ADDRicochetTargetSpawner::GetThrowDirection() const
{
	FRotator ThrowRotation = ThrowDirectionArrow != nullptr
		? ThrowDirectionArrow->GetComponentRotation()
		: GetActorRotation();

	ThrowRotation.Yaw += FMath::FRandRange(-RandomYawAngle, RandomYawAngle);
	ThrowRotation.Pitch += FMath::FRandRange(-RandomPitchAngle, RandomPitchAngle);

	return ThrowRotation.Vector().GetSafeNormal();
}

float ADDRicochetTargetSpawner::GetRandomSpeed() const
{
	const float LowerSpeed = FMath::Min(MinSpeed, MaxSpeed);
	const float UpperSpeed = FMath::Max(MinSpeed, MaxSpeed);

	return FMath::FRandRange(LowerSpeed, UpperSpeed);
}