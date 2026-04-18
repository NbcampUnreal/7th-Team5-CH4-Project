//DDRicochetProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDRicochetProjectile.generated.h"

class APlayerState;
class APawn;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class DOODOONG_API ADDRicochetProjectile : public AActor
{
	GENERATED_BODY()

public:
	ADDRicochetProjectile();

	void InitializeProjectile(AActor* InOwnerActor, APawn* InInstigatorPawn, const FVector& ShotDirection);

	APlayerState* GetThrowerPlayerState() const;

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float StartSpeed = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float MaxSpeed = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float GravityScale = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float LifeSeconds = 5.0f;
};