#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledActor.generated.h"

class USphereComponent;

UCLASS()
class DOODOONG_API APooledActor : public AActor
{
	GENERATED_BODY()

public:
	APooledActor();

	void SetInUse(bool InUse);
	bool IsInUse() const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionComp;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	static inline FVector PoolHideLocation = FVector(0.f, 0.f, -99999.f);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(ReplicatedUsing = OnRep_InUse)
	bool bInUse;

	UFUNCTION()
	virtual void OnRep_InUse();
};
