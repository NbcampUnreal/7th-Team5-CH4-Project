#pragma once

#include "CoreMinimal.h"
#include "PooledActor.h"

#include "Components/ActorComponent.h"
#include "PoolActorComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOODOONG_API UPoolActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPoolActorComponent();

protected:
	virtual void BeginPlay() override;
	void InitializePool();
	APooledActor* FindFirstAvailableActor() const;
	
public: 
	APooledActor* SpawnFromPool(const FTransform& SpawnTransform);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ObjectPooling")
	TSubclassOf<APooledActor> PooledActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ObjectPooling")
	int32 PoolSize;

	UPROPERTY()
    TArray<TObjectPtr<APooledActor>> ObjectPool;
};
