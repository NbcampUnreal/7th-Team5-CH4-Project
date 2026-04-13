#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiamondSpawner.generated.h"

class UBoxComponent;
class UPoolActorComponent;
UCLASS()
class DOODOONG_API ADiamondSpawner : public AActor
{
	GENERATED_BODY()

public:
	ADiamondSpawner();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* SpawnBox;

	UPROPERTY(VisibleAnywhere)
	UPoolActorComponent* Pool;

	UPROPERTY(EditAnywhere)
	int32 SpawnCount = 5;

	UPROPERTY(EditAnywhere)
	float SpawnInterval = 10.f;

	FTimerHandle SpawnTimer;

	void SpawnDiamonds();
	FVector GetRandomPointInBox() const;
	
public:
	UFUNCTION()
	void StartSpawn();

	UFUNCTION()
	void StopSpawn();
};
