//DDRicochetGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDRicochetGameMode.generated.h"

class ADDRicochetTargetSpawner;
class ADDRicochetCharacter;
struct FTimerHandle;

UCLASS()
class DOODOONG_API ADDRicochetGameMode : public ADDMiniGameModeBase
{
	GENERATED_BODY()

public:
	ADDRicochetGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** RicochetCharacter가 계산한 던지기 위치/회전을 기준으로 투사체를 스폰한다. */
	void ThrowProjectile(ADDRicochetCharacter* RicochetCharacter, const FVector& SpawnLocation, const FRotator& SpawnRotation);

	virtual void StartMiniGame() override;

	/** 타겟 액터를 스폰 */
	void SpawnTarget();

	bool IsMiniGameStarted() const { return bIsMiniGameStarted; }
public:
	/** 타겟 액터 스포너를 등록 */
	void RegisterTargetSpawner(ADDRicochetTargetSpawner* Spawner);

	/** 타겟 액터 스포너를 등록 해제 */
	void UnregisterTargetSpawner(ADDRicochetTargetSpawner* Spawner);

protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ADDRicochetTargetSpawner>> TargetSpawners;

	FTimerHandle TargetSpawnTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet|Target", meta = (ClampMin = "0.0"))
	float InitialTargetSpawnDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet|Target", meta = (ClampMin = "0.0"))
	float TargetSpawnInterval = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet|Target")
	int32 TargetSpawnCount = 1;

private:
	void HandleTargetSpawnTimer();
};