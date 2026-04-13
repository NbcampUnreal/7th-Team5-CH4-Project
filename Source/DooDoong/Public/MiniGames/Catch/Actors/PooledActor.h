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

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_InUse)
	bool bInUse;

	UFUNCTION()
	void OnRep_InUse();
};
