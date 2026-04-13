// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledActor.h"
#include "DiamondActor.generated.h"

UCLASS()
class DOODOONG_API ADiamondActor : public APooledActor
{
	GENERATED_BODY()

public:
	ADiamondActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
