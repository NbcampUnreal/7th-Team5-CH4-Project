#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDShooterTargetSpawner.generated.h"

class ADDShooterTarget;
class UArrowComponent;
class USceneComponent;

UCLASS()
class DOODOONG_API ADDShooterTargetSpawner : public AActor
{
	GENERATED_BODY()

public:
	ADDShooterTargetSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Shooter|Spawner")
	ADDShooterTarget* SpawnTarget();

	UFUNCTION(BlueprintCallable, Category = "Shooter|Spawner")
	FVector GetLaunchDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Shooter|Spawner")
	float GetRandomSpeed() const;

	/** 스포너가 켜져 있는지 확인 */
	UFUNCTION(BlueprintPure, Category = "Shooter|Spawner")
	bool IsEnabled() const { return bEnabled; }

protected:
	/** Scene */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	TObjectPtr<USceneComponent> SceneRoot;

	/** 발사 방향을 대략적으로 알리는 화살표. 최종 발사 각도가 랜덤으로 설정 됨에 따라 살짝 틀어질 수는 있음. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	TObjectPtr<UArrowComponent> LaunchDirectionArrow;

	/** 스폰할 타겟 액터 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	TSubclassOf<ADDShooterTarget> TargetClass;
	
	/** 나중에 특정 페이즈에서 스포너를 켜고 끌 수 있도록 하는 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	bool bEnabled = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0"))
	float MinSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0"))
	float MaxSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float RandomYawAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0", ClampMax = "89.0"))
	float RandomPitchAngle = 0.0f;
};
