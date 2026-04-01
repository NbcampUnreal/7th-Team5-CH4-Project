// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDDynamicPlatform.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class DOODOONG_API ADDDynamicPlatform : public AActor
{
	GENERATED_BODY()

public:
	ADDDynamicPlatform();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
public:
	void MovingPlatform(float DeltaTime);
	void RotatingPlatform(float DeltaTime);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootScene;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Moving")
	FVector PlatformVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Moving")
	float MoveDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Rotation")
	FRotator RotationValue;
	
private:
	FVector StartLocation;
	FVector MoveDirection;
};
