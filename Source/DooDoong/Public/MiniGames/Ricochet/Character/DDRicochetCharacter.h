//DDRicochetCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "MiniGames/Ricochet/Actors/DDRicochetProjectile.h"
#include "InputMappingContext.h"
#include "DDRicochetCharacter.generated.h"


class ADDRicochetProjectile;
class UAnimMontage;
class UCameraComponent;
class USceneComponent;
class USpringArmComponent;

UCLASS()
class DOODOONG_API ADDRicochetCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()

public:
	ADDRicochetCharacter();

public:
	virtual void BeginPlay() override;



	/** 던질 수 있는 상태인지 확인 */
	bool BIsCanThrow() const;

	/** 입력 시도. 몽타주 재생 담당 */
	UFUNCTION(BlueprintCallable, Category = "Ricochet")
	void TryThrow();

	/** GAS 이벤트가 애님몽타주에서 발사(던지기) 프레임에 도달한 경우 실제로 서버에서 실행 */
	UFUNCTION(BlueprintCallable, Category = "Ricochet")
	void HandleThrowMontageNotify();

public:
	/** 화면 중앙 기준 조준 지점 계산 */
	bool GetAimPoint(FVector& OutTargetPoint) const;

	/** 던지는 위치 반환 */
	FVector GetThrowLocation() const;

	/** 스폰 Transform 계산용 헬퍼 */
	FTransform GetProjectileSpawnTransform() const;

	/** GameMode가 실제로 스폰할 Projectile 클래스를 조회하는 헬퍼 */
	TSubclassOf<ADDRicochetProjectile> GetProjectileClass() const { return ProjectileClass; }

	/** Ability가 재생할 던지기 몽타주를 조회하는 헬퍼 */
	UAnimMontage* GetThrowMontage() const { return ThrowMontage; }


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Ricochet;

protected:

	/** 서버 RPC: 실제 던지기 처리 */
	UFUNCTION(Server, Reliable)
	void Server_TryThrow(const FVector_NetQuantize& ThrowLocation, const FVector_NetQuantize& TargetPoint);

	/** 던지기 몽타주 재생 */
	void PlayThrowMontage();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<USceneComponent> ThrowComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ricochet")
	TObjectPtr<UCameraComponent> CameraComp;

protected:
	/** 던지기 투사체 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet")
	TSubclassOf<ADDRicochetProjectile> ProjectileClass;

	/** 던지기 애니메이션 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ThrowMontage;

	/** 소켓 이름 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FName ProjectileSocketName = TEXT("RightHand_ThrowSocket");

	/** 조준 트레이스 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ricochet")
	float AimTraceDistance = 5000.0f;
};