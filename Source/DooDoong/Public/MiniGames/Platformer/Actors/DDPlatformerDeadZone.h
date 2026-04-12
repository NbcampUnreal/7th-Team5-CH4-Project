#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDPlatformerDeadZone.generated.h"

UCLASS()
class DOODOONG_API ADDPlatformerDeadZone : public AActor
{
	GENERATED_BODY()

public:
	ADDPlatformerDeadZone();

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class USceneComponent> SceneRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UBoxComponent> BoxCollision;

private:
	
	FVector StartLocation;
};
