#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDShooterTarget.generated.h"

class ADDShotProjectile;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class DOODOONG_API ADDShooterTarget : public AActor
{
	GENERATED_BODY()

public:
	ADDShooterTarget();

	/** 서버에서 투사체 충돌을 처리하고, 발사자의 PlayerState를 기준으로 점수를 지급한다. */
	bool HandleProjectileHit(ADDShotProjectile* ShotProjectile);

protected:
	virtual void BeginPlay() override;

protected:
	/** 탄환 Overlap 판정용 콜리전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UBoxComponent> CollisionComp;

	/** 표적 외형 표시용 메시. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	/** 이 표적을 맞혔을 때 발사자에게 부여할 점수. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter")
	int32 ScoreValue = 1;

	/** 중복 점수 지급 방지. 한 번 맞으면 false */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Shooter")
	bool bCanGetScore = true;
};
