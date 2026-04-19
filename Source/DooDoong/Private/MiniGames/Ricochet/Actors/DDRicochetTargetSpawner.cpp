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

	// 발사 방향
	const FVector ThrowDirection = GetThrowDirection();

	// 스폰 위치
	const FVector Forward = ThrowDirectionArrow != nullptr
		? ThrowDirectionArrow->GetForwardVector()
		: GetActorForwardVector();

	const FVector Right = ThrowDirectionArrow != nullptr
		? ThrowDirectionArrow->GetRightVector()
		: GetActorRightVector();

	const FVector SpawnLocation =
		GetActorLocation()
		+ Right * FMath::RandRange(-50.f, 50.f)
		+ FVector(0.f, 0.f, FMath::RandRange(10.f, 30.f));

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADDRicochetTarget* Target = GetWorld()->SpawnActor<ADDRicochetTarget>(
		TargetClass,
		SpawnLocation,
		ThrowDirection.Rotation(),
		SpawnParams);

	UE_LOG(LogTemp, Warning, TEXT("Spawned Target: %s at %s"),
		*GetNameSafe(Target),
		*SpawnLocation.ToString());

	if (Target != nullptr)
	{
		if (HasAuthority())
		{
			//  핵심: 속도 통일 (랜덤 최소만)
			const float Speed = FMath::FRandRange(
				MaxSpeed * 0.9f,
				MaxSpeed
			);

			Target->ThrowTarget(ThrowDirection, Speed);
		}
	}

	return Target;
}









FVector ADDRicochetTargetSpawner::GetThrowDirection() const
{
	FRotator ThrowRotation = ThrowDirectionArrow != nullptr
		? ThrowDirectionArrow->GetComponentRotation()
		: GetActorRotation();

	// 기본 위로 쏘기 (핵심)
	ThrowRotation.Pitch += 25.f;

	// 랜덤 퍼짐
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