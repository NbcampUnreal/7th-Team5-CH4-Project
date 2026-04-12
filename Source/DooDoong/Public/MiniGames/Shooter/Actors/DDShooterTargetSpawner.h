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

	UFUNCTION(BlueprintCallable, Category = "Shooter|Spawner")
	ADDShooterTarget* SpawnTarget();

	UFUNCTION(BlueprintCallable, Category = "Shooter|Spawner")
	FVector GetLaunchDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Shooter|Spawner")
	float GetRandomSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Shooter|Spawner")
	bool IsEnabled() const { return bEnabled; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	TObjectPtr<UArrowComponent> LaunchDirectionArrow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	TSubclassOf<ADDShooterTarget> TargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0"))
	float MinSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0"))
	float MaxSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float RandomYawAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter|Spawner", meta = (ClampMin = "0.0", ClampMax = "89.0"))
	float RandomPitchAngle = 0.0f;
};
