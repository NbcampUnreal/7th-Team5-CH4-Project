#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDShotProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class DOODOONG_API ADDShotProjectile : public AActor
{
	GENERATED_BODY()

public:
	ADDShotProjectile();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USphereComponent> CollisionComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;
	
private:
	float StartSpeed = 2000.0f;
	float MaxSpeed = 2000.0f;
	float GravityScale = 0.0f;
};
