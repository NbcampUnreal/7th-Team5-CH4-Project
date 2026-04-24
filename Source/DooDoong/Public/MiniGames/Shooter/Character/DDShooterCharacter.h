#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDShooterCharacter.generated.h"

class ADDShotProjectile;
class UAnimMontage;
class UCameraComponent;
class USceneComponent;
class USpringArmComponent;

UCLASS()
class DOODOONG_API ADDShooterCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()

public:
	ADDShooterCharacter();

public:
	virtual void BeginPlay() override;

	/** 발사 가능 여부를 확인하는 헬퍼.  */
	bool BIsCanFire() const;

	/** 발사 입력 시도. 몽타주 재생 담당 */
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void TryFire();

	/** GAS 이벤트가 애님몽타주에서 발사 프레임에 도달한 경우 실제로 서버에서 발사 */
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void HandleFireMontageNotify();

public:
	/** 화면 중앙 기준 조준 지점 계산 */
	bool GetAimPoint(FVector& OutTargetPoint) const;

	/** 발사 위치를 반환하는 헬퍼 */
	FVector GetMuzzleLocation() const;

	/** 소켓 기준 발사체 스폰 Transform 계산용 헬퍼 */
	FTransform GetProjectileSpawnTransform() const;

	/** GameMode가 실제로 스폰할 Projectile 클래스를 조회하는 헬퍼 */
	TSubclassOf<ADDShotProjectile> GetProjectileClass() const { return ProjectileClass; }

	/** Ability가 재생할 발사 몽타주를 조회하는 헬퍼 */
	UAnimMontage* GetFireMontage() const { return FireMontage; }

protected:
	/** 클라이언트가 계산한 발사 위치/방향을 서버에 넘겨 실제로 발사 */
	UFUNCTION(Server, Reliable)
	void Server_TryFire(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& TargetPoint);

	/** 발사 몽타주 재생 */
	void PlayFireMontage();

protected:
	/** 소켓이 없을 때 발사 위치 대체 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USceneComponent> MuzzleComp;

	/** 3인칭 Shooter 시점용 스프링 암 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	/** 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UCameraComponent> CameraComp;

protected:
	/** 발사체 클래스. 없으면 발사 불가 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	TSubclassOf<ADDShotProjectile> ProjectileClass;

	/** 있으면 발사 애니메이션을 먼저 재생, 없으면 즉시 발사함 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	/** 발사할 위치를 읽어올 스켈레탈 메시의 소켓 이름. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Animation")
	FName ProjectileSocketName = TEXT("RightHand_ProjectileSocket");

	/** 트레이스의 최대거리 맞은 지점이 없으면 이 거리 끝점을 조준점으로 사용. 일단 엄청 길게 해둠 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	float AimTraceDistance = 5000.0f;
};
