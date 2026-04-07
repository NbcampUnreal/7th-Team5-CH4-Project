#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDShooterCharacter.generated.h"

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
	
	bool CanFire() const;
	void TryFire();
	bool GetCrosshairAimPoint(FVector& OutTargetPoint) const;
	FVector GetMuzzleLocation() const;

protected:
	UFUNCTION(Server, Reliable)
	void Server_TryFire(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& TargetPoint);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USceneComponent> MuzzleComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UCameraComponent> CameraComp;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	float AimTraceDistance = 5000.0f;
};
