#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DDRicochetGameState.generated.h"

class ADDRicochetTargetSpawner;

UCLASS()
class ADDRicochetGameState : public AGameStateBase
{
    GENERATED_BODY()

public:

    void RegisterSpawner(ADDRicochetTargetSpawner* Spawner);
    void UnregisterSpawner(ADDRicochetTargetSpawner* Spawner);

    const TArray<TObjectPtr<ADDRicochetTargetSpawner>>& GetSpawners() const
    {
        return TargetSpawners;
    }

private:

    UPROPERTY()
    TArray<TObjectPtr<ADDRicochetTargetSpawner>> TargetSpawners;
};