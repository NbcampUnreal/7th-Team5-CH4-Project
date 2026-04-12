#include "MiniGames/Shooter/GameMode/DDShooterGameMode.h"

#include "MiniGames/Shooter/Actors/DDShooterTargetSpawner.h"
#include "MiniGames/Shooter/Actors/DDShotProjectile.h"
#include "MiniGames/Shooter/Character/DDShooterCharacter.h"
#include "MiniGames/Shooter/GameState/DDShooterGameState.h"
#include "TimerManager.h"

ADDShooterGameMode::ADDShooterGameMode()
{
	GameStateClass = ADDShooterGameState::StaticClass();
}

void ADDShooterGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ADDShooterGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TargetSpawnTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ADDShooterGameMode::FireProjectile(ADDShooterCharacter* ShooterCharacter, const FVector& SpawnLocation,
                                        const FRotator& SpawnRotation)
{
	if (ShooterCharacter == nullptr || GetWorld() == nullptr)
	{
		return;
	}

	const TSubclassOf<ADDShotProjectile> ProjectileClass = ShooterCharacter->GetProjectileClass();
	if (ProjectileClass == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = ShooterCharacter;
	SpawnParams.Instigator = ShooterCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADDShotProjectile* Projectile = GetWorld()->SpawnActor<ADDShotProjectile>(ProjectileClass, SpawnLocation,
		SpawnRotation, SpawnParams);
	if (Projectile == nullptr)
	{
		return;
	}

	Projectile->InitializeShot(ShooterCharacter, ShooterCharacter, SpawnRotation.Vector());
}

void ADDShooterGameMode::StartMiniGame()
{
	Super::StartMiniGame();

	if (!HasAuthority() || !bIsMiniGameStarted || bIsMiniGameFinished || GetWorld() == nullptr)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TargetSpawnTimerHandle);

	if (TargetSpawnInterval <= 0.0f)
	{
		SpawnTarget();
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		TargetSpawnTimerHandle,
		this,
		&ADDShooterGameMode::HandleTargetSpawnTimer,
		TargetSpawnInterval,
		true,
		InitialTargetSpawnDelay);
}

void ADDShooterGameMode::SpawnTarget()
{
	if (!HasAuthority())
	{
		return;
	}

	TargetSpawners.RemoveAll([](const TObjectPtr<ADDShooterTargetSpawner>& Spawner)
	{
		return !IsValid(Spawner);
	});

	TArray<ADDShooterTargetSpawner*> EnabledSpawners;
	EnabledSpawners.Reserve(TargetSpawners.Num());

	for (ADDShooterTargetSpawner* Spawner : TargetSpawners)
	{
		if (Spawner != nullptr && Spawner->IsEnabled())
		{
			EnabledSpawners.Add(Spawner);
		}
	}

	if (EnabledSpawners.Num() == 0)
	{
		return;
	}

	for (int i = 0; i < TargetSpawnCount; i++)
	{
		const int32 SpawnerIndex = FMath::RandRange(0, EnabledSpawners.Num() - 1);
		EnabledSpawners[SpawnerIndex]->SpawnTarget();
	}
}

void ADDShooterGameMode::RegisterTargetSpawner(ADDShooterTargetSpawner* Spawner)
{
	if (!HasAuthority() || Spawner == nullptr)
	{
		return;
	}

	TargetSpawners.AddUnique(Spawner);
}

void ADDShooterGameMode::UnregisterTargetSpawner(ADDShooterTargetSpawner* Spawner)
{
	if (!HasAuthority() || Spawner == nullptr)
	{
		return;
	}

	TargetSpawners.Remove(Spawner);
}

void ADDShooterGameMode::HandleTargetSpawnTimer()
{
	if (!HasAuthority() || !bIsMiniGameStarted || bIsMiniGameFinished)
	{
		return;
	}

	SpawnTarget();
}
