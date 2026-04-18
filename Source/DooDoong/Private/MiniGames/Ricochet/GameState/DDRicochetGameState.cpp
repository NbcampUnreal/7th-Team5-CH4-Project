#include "MiniGames/Ricochet/GameState/DDRicochetGameState.h"
#include "MiniGames/Ricochet/Actors/DDRicochetTargetSpawner.h"

void ADDRicochetGameState::RegisterSpawner(ADDRicochetTargetSpawner* Spawner)
{
    if (!IsValid(Spawner))
        return;

    TargetSpawners.AddUnique(Spawner);

    UE_LOG(LogTemp, Warning, TEXT("[GS] Registered Spawner: %s | Total: %d"),
        *Spawner->GetName(),
        TargetSpawners.Num());
}

void ADDRicochetGameState::UnregisterSpawner(ADDRicochetTargetSpawner* Spawner)
{
    TargetSpawners.Remove(Spawner);

    UE_LOG(LogTemp, Warning, TEXT("[GS] Unregistered Spawner | Total: %d"),
        TargetSpawners.Num());
}