#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDShotProjectile.generated.h"

class APlayerState;
class APawn;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class DOODOONG_API ADDShotProjectile : public AActor
{
	GENERATED_BODY()

public:
	ADDShotProjectile();

	/** 발사 직후 소유자, Instigator, 초기 속도를 세팅한다. */
	void InitializeShot(AActor* InOwnerActor, APawn* InInstigatorPawn, const FVector& ShotDirection);

	/** 점수 지급 시 사용할 발사자의 PlayerState를 반환한다. */
	APlayerState* GetShooterPlayerState() const;

protected:
	virtual void BeginPlay() override;

	/** Actor Begin Overlap 사용 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = true))
	float StartSpeed = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = true))
	float MaxSpeed = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = true))
	float GravityScale = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = true))
	float LifeSeconds = 5.0f;
};
