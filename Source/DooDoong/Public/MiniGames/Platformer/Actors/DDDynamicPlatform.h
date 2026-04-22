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
	
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void MovingPlatform(float DeltaSeconds);
	UFUNCTION()
	void RotatingPlatform(float DeltaSeconds);
	
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
