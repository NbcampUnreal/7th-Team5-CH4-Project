// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledActor.generated.h"

UCLASS()
class DOODOONG_API APooledActor : public AActor
{
	GENERATED_BODY()

public:
	APooledActor();

	void SetInUse(bool _InUse);
	bool IsInUse() const;
	
protected:
	virtual void BeginPlay() override;

private:
	bool bInUse;
};
