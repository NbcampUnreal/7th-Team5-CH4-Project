//DDRicochetTarget.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "DDRicochetTarget.generated.h"

class UProjectileMovementComponent;
class ADDRicochetProjectile;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class DOODOONG_API ADDRicochetTarget : public AActor
{
	GENERATED_BODY()

public:
	ADDRicochetTarget();

	virtual void BeginPlay() override;

public:
	/** 타겟 액터를 스폰하고 특정한 방향으로 던진다 */
	void ThrowTarget(const FVector& ThrowDirection, float Speed);

	/** 서버에서 투사체 충돌을 처리하고, 발사자의 PlayerState를 기준으로 점수를 지급한다. */
	bool HandleProjectileHit(ADDRicochetProjectile* RicochetProjectile);

protected:
	/** 탄환 Overlap 판정용 콜리전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<UBoxComponent> CollisionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	UNiagaraSystem* HitEffect;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHitFX();

	/** 표적 외형 표시용 메시. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	/** 타겟을 이동시키기 위한 ProjectileMovementComponent */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float StartSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float MaxSpeed = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float GravityScale = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet", meta = (AllowPrivateAccess = true))
	float LifeSeconds = 5.0f;

protected:
	/** 이 표적을 맞혔을 때 발사자에게 부여할 점수. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ricochet")
	int32 ScoreValue = 1;

	/** 중복 점수 지급 방지. 한 번 맞으면 false */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ricochet")
	bool bCanGetScore = true;
};