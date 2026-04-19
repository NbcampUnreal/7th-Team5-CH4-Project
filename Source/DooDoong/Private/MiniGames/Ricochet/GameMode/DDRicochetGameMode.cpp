//DDRicochetGameMode.cpp
#include "MiniGames/Ricochet/GameMode/DDRicochetGameMode.h"
#include "EngineUtils.h"
#include "MiniGames/Ricochet/Actors/DDRicochetTargetSpawner.h"
#include "MiniGames/Ricochet/Actors/DDRicochetProjectile.h"
#include "MiniGames/Ricochet/Character/DDRicochetCharacter.h"
#include "MiniGames/Ricochet/GameState/DDRicochetGameState.h"
#include "TimerManager.h"

ADDRicochetGameMode::ADDRicochetGameMode()
{
	GameStateClass = ADDRicochetGameState::StaticClass();
}

void ADDRicochetGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	if (HasAuthority())////////////
	{
		bIsMiniGameStarted = false; // 혹시 모르니까 초기화
		StartMiniGame();//////////////////////////////////
	}

	TargetSpawners.Empty();

	if (UWorld* World = GetWorld())
	{
		if (ADDRicochetGameState* GS = World->GetGameState<ADDRicochetGameState>())
		{
			TargetSpawners = GS->GetSpawners();

			UE_LOG(LogTemp, Warning, TEXT("[GM] Loaded Spawners: %d"),
				TargetSpawners.Num());
		}
	}
}

void ADDRicochetGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TargetSpawnTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ADDRicochetGameMode::ThrowProjectile(
	ADDRicochetCharacter* RicochetCharacter,
	const FVector& SpawnLocation,
	const FRotator& SpawnRotation)
{
	if (RicochetCharacter == nullptr || GetWorld() == nullptr)
	{
		return;
	}

	const TSubclassOf<ADDRicochetProjectile> ProjectileClass = RicochetCharacter->GetProjectileClass();
	if (ProjectileClass == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RicochetCharacter;
	SpawnParams.Instigator = RicochetCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADDRicochetProjectile* Projectile = GetWorld()->SpawnActor<ADDRicochetProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	UE_LOG(LogTemp, Warning, TEXT("[Projectile Spawn] NetMode=%d HasAuthority=%d"),
		GetWorld()->GetNetMode(),
		HasAuthority());

	if (Projectile == nullptr)
	{
		return;
	}

	Projectile->InitializeProjectile(
		RicochetCharacter,
		RicochetCharacter,
		SpawnRotation.Vector());
}






void ADDRicochetGameMode::StartMiniGame()
{
	Super::StartMiniGame();

	UE_LOG(LogTemp, Warning, TEXT("[Ricochet] StartMiniGame CALLED"));

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("[Ricochet] No Authority"));
		return;
	}

	if (!bIsMiniGameStarted)
	{
		UE_LOG(LogTemp, Error, TEXT("[Ricochet] bIsMiniGameStarted = FALSE"));
		return;
	}

	if (bIsMiniGameFinished)
	{
		UE_LOG(LogTemp, Error, TEXT("[Ricochet] Already Finished"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Ricochet] StartMiniGame PASSED CHECK"));

	// 기존 타이머 제거
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TargetSpawnTimerHandle);
	}

	//  스폰 주기 없으면 1회 즉시 실행
	if (TargetSpawnInterval <= 0.f)
	{
		SpawnTarget();
		return;
	}

	//  타이머 시작 (핵심)
	GetWorld()->GetTimerManager().SetTimer(
		TargetSpawnTimerHandle,
		this,
		&ADDRicochetGameMode::HandleTargetSpawnTimer,
		TargetSpawnInterval,
		true,
		InitialTargetSpawnDelay
	);

	UE_LOG(LogTemp, Warning, TEXT("[Ricochet] Timer Started"));
}








void ADDRicochetGameMode::SpawnTarget()
{
	if (!HasAuthority())
		return;

	UE_LOG(LogTemp, Warning, TEXT("[GM] SpawnTarget CALLED"));

	TargetSpawners.RemoveAll([](const TObjectPtr<ADDRicochetTargetSpawner>& Spawner)
		{
			return !IsValid(Spawner);
		});

	TArray<ADDRicochetTargetSpawner*> EnabledSpawners;

	for (ADDRicochetTargetSpawner* Spawner : TargetSpawners)
	{
		if (Spawner && Spawner->IsEnabled())
		{
			EnabledSpawners.Add(Spawner);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[GM] Enabled Spawners: %d"), EnabledSpawners.Num());

	if (EnabledSpawners.Num() == 0)
		return;

	for (int32 i = 0; i < TargetSpawnCount; i++)
	{
		const int32 Index = FMath::RandRange(0, EnabledSpawners.Num() - 1);
		EnabledSpawners[Index]->SpawnTarget();
	}
}




void ADDRicochetGameMode::RegisterTargetSpawner(ADDRicochetTargetSpawner* Spawner)
{
	if (!HasAuthority() || Spawner == nullptr)
	{
		return;
	}

	TargetSpawners.AddUnique(Spawner);

	UE_LOG(LogTemp, Warning, TEXT("[GM] Spawner Registered: %s | Total: %d"),
		*Spawner->GetName(),
		TargetSpawners.Num());
}



void ADDRicochetGameMode::UnregisterTargetSpawner(ADDRicochetTargetSpawner* Spawner)
{
	if (!HasAuthority() || Spawner == nullptr)
	{
		return;
	}

	TargetSpawners.Remove(Spawner);
}






void ADDRicochetGameMode::HandleTargetSpawnTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("[Ricochet] Timer Tick"));

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("[Ricochet] Timer No Authority"));
		return;
	}

	if (!bIsMiniGameStarted || bIsMiniGameFinished)
	{
		UE_LOG(LogTemp, Error, TEXT("[Ricochet] Timer Blocked by GameState"));
		return;
	}

	SpawnTarget();
}