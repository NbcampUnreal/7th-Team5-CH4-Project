//DDRicochetTargetSpawner.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDRicochetTargetSpawner.generated.h"

class ADDRicochetTarget;
class UArrowComponent;
class USceneComponent;

UCLASS()
class DOODOONG_API ADDRicochetTargetSpawner : public AActor
{
	GENERATED_BODY()

public:
	ADDRicochetTargetSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Ricochet|Spawner")
	ADDRicochetTarget* SpawnTarget();

	UFUNCTION(BlueprintCallable, Category = "Ricochet|Spawner")
	FVector GetThrowDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Ricochet|Spawner")
	float GetRandomSpeed() const;

	/** 스포너가 켜져 있는지 확인 */
	UFUNCTION(BlueprintPure, Category = "Ricochet|Spawner")
	bool IsEnabled() const { return bEnabled; }

protected:
	/** Scene */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner")
	TObjectPtr<USceneComponent> SceneRoot;

	/** 던지는 방향을 대략적으로 알리는 화살표. 최종 방향은 랜덤으로 약간 보정됨 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner")
	TObjectPtr<UArrowComponent> ThrowDirectionArrow;

	/** 스폰할 타겟 액터 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner")
	TSubclassOf<ADDRicochetTarget> TargetClass;

	/** 나중에 특정 페이즈에서 스포너를 켜고 끌 수 있도록 하는 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner")
	bool bEnabled = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner", meta = (ClampMin = "0.0"))
	float MinSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner", meta = (ClampMin = "0.0"))
	float MaxSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float RandomYawAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ricochet|Spawner", meta = (ClampMin = "0.0", ClampMax = "89.0"))
	float RandomPitchAngle = 0.0f;
};