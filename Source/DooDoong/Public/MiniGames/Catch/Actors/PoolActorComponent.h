// Fill out your copyright notice in the Description page of Project Settings.

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
	APooledActor* SpawnFromPool(const FTransform& SpawnTransform) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ObjectPool")
	TSubclassOf<APooledActor> PooledActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintCallable, Category="ObjectPool")
	int32 PoolSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ObjectPool")
	TArray<APooledActor*> ObjectPool;
};
