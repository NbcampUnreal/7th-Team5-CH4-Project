#include "MiniGames/Shooter/GameMode/DDShooterGameMode.h"

#include "MiniGames/Shooter/Actors/DDShotProjectile.h"
#include "MiniGames/Shooter/Character/DDShooterCharacter.h"
#include "MiniGames/Shooter/GameState/DDShooterGameState.h"

ADDShooterGameMode::ADDShooterGameMode()
{
	GameStateClass = ADDShooterGameState::StaticClass();
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
