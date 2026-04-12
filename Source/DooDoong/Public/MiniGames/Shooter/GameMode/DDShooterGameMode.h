#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDShooterGameMode.generated.h"

class ADDShooterTargetSpawner;
class ADDShooterCharacter;
struct FTimerHandle;

UCLASS()
class DOODOONG_API ADDShooterGameMode : public ADDMiniGameModeBase
{
	GENERATED_BODY()

public:
	ADDShooterGameMode();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** ShooterCharacter가 계산한 발사 위치/회전을 기준으로 발사체를 스폰한다. */
	void FireProjectile(ADDShooterCharacter* ShooterCharacter, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	virtual void StartMiniGame() override;
	
	/** 타겟 액터를 스폰 */
	void SpawnTarget();

public:
	/** 타겟 액터 스포너를 등록 */
	void RegisterTargetSpawner(ADDShooterTargetSpawner* Spawner);
	
	/** 타겟 액터 스포너를 등록 해제 */
	void UnregisterTargetSpawner(ADDShooterTargetSpawner* Spawner);
	
protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ADDShooterTargetSpawner>> TargetSpawners;

	FTimerHandle TargetSpawnTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Target", meta = (ClampMin = "0.0"))
	float InitialTargetSpawnDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Target", meta = (ClampMin = "0.0"))
	float TargetSpawnInterval = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Target")
	int32 TargetSpawnCount = 3;

private:
	void HandleTargetSpawnTimer();
};
