#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDPlatformerGoalPoint.generated.h"

UCLASS()
class DOODOONG_API ADDPlatformerGoalPoint : public AActor
{
	GENERATED_BODY()

public:
	ADDPlatformerGoalPoint();

public:
	
	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void ServerNotifyGoalReached();

	
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
};
